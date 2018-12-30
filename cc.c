#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

enum{
  TK_NUM = 256,
  TK_IDENT,
  TK_EOF,
};

enum {
  ND_NUM = 256,
  ND_IDENT,
};

typedef struct {
  int ty;
  int val;
  char *input;
} Token;

typedef struct Node{
  int ty;
  struct Node *lhs;
  struct Node *rhs;
  int val;
  char name;
} Node;

void error(int i);
void tokenize(char *p);
Node *new_node(int ty, Node *lhs, Node *rhs);
Node *new_node_num(int val);
void program();
Node *assign();
Node *term();
Node *mul();
Node *expr();
void gen_lval(Node *node);
void gen(Node *node);


Token tokens[100];
Node *code[100];
int pos = 0;
int code_pos = 0;


void error(int i){
  fprintf(stderr, "Invalid token: %s\n", tokens[i].input);
  exit(1);
}

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

Node *term(){
  if(tokens[pos].ty == TK_NUM || tokens[pos].ty == TK_IDENT)
    return new_node_num(tokens[pos++].val);
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
  if(tokens[pos].ty == ';' || tokens[pos].ty == TK_EOF) return lhs;
  if(tokens[pos].ty == '=') {
    pos++;
    return new_node('=', lhs, assign());
  }
  error(*tokens[pos].input);
}

void gen_lval(Node *node){
  if(node->ty == ND_IDENT){
    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n",('z' - node->name + 1) * 8);
    printf("  push rax\n");
    return;
  }
  fprintf(stderr, "lhs is not variable\n");
  exit(EXIT_FAILURE);
}


void gen(Node *node){
  if(node->ty == ND_NUM){
    printf("  push %d\n",node->val);
    return;
  }

  if(node->ty == ND_NUM) {
    printf("  push %d\n",node->val);
    return;
  }

  if(node->ty == ND_IDENT) {
    gen_lval(node);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  }

  if(node->ty == ND_IDENT) {
    gen_lval(node);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  }

  if(node->ty == '='){
    gen_lval(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  mov [rax], rdi\n");
    printf("  push rdi\n");
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->ty) {
    case '+':
      printf("  add rax, rdi\n");
      break;
    case '-':
      printf("  sub rax, rdi\n");
      break;
    case '*':
      printf("  mul rdi\n");
      break;
    case '/':
      printf("  mov rdx, 0\n");
      printf("  div rdi\n");
      break;
  }

  printf("  push rax\n");
}


int main(int argc, char *argv[])
{
  if(argc != 2)
  {
    fprintf(stderr, "Invalid arg num\n");
    return 1;
  }

  tokenize(argv[1]);
  program();

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // prologue
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, 208\n");

  for (int i = 0; code[i]; i++){
    gen(code[i]);
    printf("  pop rax\n");
  }

  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");

  return 0;
}
