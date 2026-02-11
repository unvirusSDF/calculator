#include "header.h"

uint32_t log2_uint(uint32_t n){
  if(!n) return ~(uint32_t)0;
  uint32_t log=31;
  for(;!(n&(1<<log));log--);
  return log;
}

// spatial:
//  aveage O(log n)
//  worse  O(n)
//  best   O(1)

// time
//   average O(n)
//   worse   O(n)
//   best    O(n)

uint32_t eval_expr_n(const expr_t e, uint16_t n){

  uint16_t stack_ptr={0x0};
  const uint16_t stack_cap={1+ n/2}; // only the half because most operators are binary, so for n operators there would be at worse n+1 numbers to store
  DEBUG(
      printf("evaluating [ "); print_tree(e); printf("] of size %hu\n",n);
      printf("1+n/2 : %hu\n", stack_cap);
    );

  uint32_t* buffer_stack = malloc(stack_cap*sizeof(uint32_t));

#define PUSH(a) buffer_stack[stack_ptr++]=a;
#define POP() buffer_stack[--stack_ptr];

  for(uint16_t i=n-1;;i--){

    // this is a match, change my mind
    switch(e[i].type){
      case _null: break;

      case _uint32_t:
        PUSH(e[i].data._uint32_t);
        break;

      case op_plus:
        {
          const uint32_t a= POP();
          const uint32_t b= POP();
          PUSH(a+b);
        }
        break;

      case op_minus:
        {
          const uint32_t a= POP();
          const uint32_t b= POP();
          PUSH(a-b);
        }
        break;

      case op_asterisk:
        {
          const uint32_t a= POP();
          const uint32_t b= POP();
          PUSH(a*b);
        }
        break;

      case op_slash:
        {
          const uint32_t a= POP();
          const uint32_t b= POP();
          PUSH(a/b);
        }
        break;

      case op_percent:
        {
          const uint32_t a= POP();
          const uint32_t b= POP();
          PUSH(a%b);
        }
        break;

      case op_circumflex:
        {
          const uint32_t a= POP();
          const uint32_t b= POP();
          PUSH(a^b);
        }
        break;

      case op_pipe:
        {
          const uint32_t a= POP();
          const uint32_t b= POP();
          PUSH(a|b);
        }
        break;
      case op_amperstand:
        {
          const uint32_t a= POP();
          const uint32_t b= POP();
          PUSH(a&b);
        }
        break;
    }

    DEBUG(
        printf("i : %hu, reverse stack:\n[", i);
        for(uint16_t i=0; i<stack_ptr;i++){
          printf("%hu ", buffer_stack[i]);
        }
        printf("]<\n");
        fflush(stdout);
      );

    if(!i) break;
  }

  uint32_t result={};
  if(stack_ptr) result = POP();

  free(buffer_stack);
#undef PUSH
#undef POP

  return result;
}


uint32_t eval_expr(const expr_t e){
  uint16_t n=0;
  for(;e[n].type;n++);
  return eval_expr_n(e,n);
}

/*
brouillon:

let rec eval expr =
  match expr with
  | Number(val) -> val
  | Expr(op, lhs, rhs) -> (operator_fn op) (eval lhs) (eval rhs)
;;


idea : run through the tree by the bottom
test:
  + 25 - + 15 8 + 5 12

-> + 25 - + 15 8 17
-> + 25 - 23 17
-> + 25 6
-> 31

solution (potential):
  pointer from back of the tree
  linked list of val for the one after the pointer

example:
  + 25 - + 15 8 + 5 12

-> + 25 - + 15 8 [17] // push 17 (aka + 5 12) in the list
-> + 25 - [23 17] // push 23 (aka + 15 8) in the list
-> + 25 [6] // front of the list used as the missing variable
-> [31] // final res as the only element of the list


test 2:
  + + + 1 2 3 4
-> + + [3 3 4]
-> + [6 4]
-> [10]





























*/
