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
Node *new_node_ident(char name);
void program();
Node *assign();
Node *term();
Node *mul();
Node *expr();
void gen_lval(Node *node);
void gen(Node *node);

extern Token tokens[];
extern Node *code[];
extern int pos;
extern int code_pos;
