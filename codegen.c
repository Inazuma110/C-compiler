#include "cc.h"

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
