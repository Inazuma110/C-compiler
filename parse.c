#include "cc.h"

int pos = 0;
int code_pos = 0;
Token tokens[100];

void tokenize(char *p) {
  int i = 0;
  while(*p){
    if (isspace(*p)){
      p++;
      continue;
    }

    if(*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')' || *p == '=' || *p == ';' ){
      tokens[i].ty = *p;
      tokens[i].input = p;
      i++;
      p++;
      continue;
    }

    if(isdigit(*p)){
      tokens[i].ty = TK_NUM;
      tokens[i].input = p;
      tokens[i].val = strtol(p, &p, 10);
      i++;
      continue;
    }

    if(*p >= 'a' && *p <= 'z')
    {
      tokens[i].ty = TK_IDENT;
      tokens[i].input = p;
      i++;
      p++;
      continue;
    }

    fprintf(stderr, "cannot tokenize: %s\n", p);
    exit(1);
  }
  tokens[i].ty = TK_EOF;
  tokens[i].input = p;
}

Node *new_node(int ty, Node *lhs, Node *rhs){
  Node *node = malloc(sizeof(Node));
  node->ty = ty;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val){
  Node *node = malloc(sizeof(Node));
  node->ty = ND_NUM;
  node->val = val;
  return node;
}

Node *new_node_ident(char name){
  Node *node = malloc(sizeof(Node));
  node->ty = ND_IDENT;
  node->name = name;
  return node;
}

Node *term(){
  if(tokens[pos].ty == TK_NUM)
    return new_node_num(tokens[pos++].val);
  if(tokens[pos].ty == TK_IDENT)
    return new_node_ident(*tokens[pos++].input);

  if(tokens[pos].ty == '(')
  {
    pos++;
    Node *node = expr();
    if(tokens[pos].ty != ')')
      error(*tokens[pos].input);
    pos++;
    return node;
  }
  error(*tokens[pos].input);
  return NULL;
}

Node *expr() {
  Node *lhs = mul();
  if(tokens[pos].ty == '+'){
    pos++;
    return new_node('+', lhs, expr());
  }
  if(tokens[pos].ty == '-'){
    pos++;
    return new_node('-', lhs, expr());
  }
  return lhs;
}

Node *mul(){
  Node *lhs = term();
  if(tokens[pos].ty == '*'){
    pos++;
    return new_node('*', lhs, mul());
  }
  if(tokens[pos].ty == '/'){
    pos++;
    return new_node('/', lhs, mul());
  }
  return lhs;
}

void program(){
  if(tokens[pos].ty == TK_EOF)
  {
    code[code_pos] = NULL;
    return;
  }
  Node *lhs = assign();
  code[code_pos] = lhs;
  code_pos++;
  program();
}


Node* assign(){
  Node *lhs = expr();

  if(tokens[pos].ty == '='){
    pos++;
    return new_node('=', lhs, assign());
  }
  if(tokens[pos].ty == ';'){
    pos++;
    return lhs;
  } else {
    fprintf(stderr, "not found semicolon.\n");
    exit(EXIT_FAILURE);
  }

  error(*tokens[pos].input);
}
