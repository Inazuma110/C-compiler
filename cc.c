#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


enum{
  TK_NUM = 256,
  TK_EOF,
};

enum {
  ND_NUM = 256,
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
} Node;

Node* expr();
Token tokens[100];
int pos = 0;

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

    if(*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')'){
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
  if(tokens[pos].ty == TK_NUM)
    return new_node_num(tokens[pos++].val);
  if(tokens[pos].ty == '(')
  {
    pos++;
    Node *node = expr();
    if(tokens[pos].ty != ')')
      error(tokens[pos].input);
    pos++;
    return node;
  }
  error(tokens[pos].input);
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

void gen(Node *node){
  if(node->ty == ND_NUM){
    printf("  push %d\n",node->val);
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


int main(int argc, char const* argv[])
{
  if(argc != 2)
  {
    fprintf(stderr, "Invalid arg num\n");
    return 1;
  }

  tokenize(argv[1]);
  Node* node = expr();

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  gen(node);

  //
  // if(tokens[0].ty != TK_NUM)
  //   error(0);
  // printf("  mov rax, %d\n", tokens[0].val);
  // int i = 1;
  //
  // while(tokens[i].ty != TK_EOF)
  // {
  //   if(tokens[i].ty == '+'){
  //     i++;
  //     if(tokens[i].ty != TK_NUM) error(i);
  //     printf("  add rax, %d\n", tokens[i].val);
  //     i++;
  //     continue;
  //   }
  //
  //   if(tokens[i].ty == '-')
  //   {
  //     i++;
  //     if(tokens[i].ty != TK_NUM) error(i);
  //     printf("  sub rax, %d\n", tokens[i].val);
  //     i++;
  //     continue;
  //   }
  //
  //   error(i);
  // }

  printf("  pop rax\n");
  printf("  ret\n");
  return 0;
}
