#pragma once

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>


/*  plan:
 *  everything on a stack with infixed operators and functions calls
 *
 *  operator : enum on char (ascii value)
 *
 *  value    : tagged union of things like floats and ints (maybe vec), for now it's only uint32_t for simplicity
 *
 *
 *
 *
 *
 */


#define OPERATOR_X_LIST X('+', plus) X('-', minus) X('*', asterisk) X('/', slash) X('%', percent) X('^', circumflex) X('|', pipe) X('&', amperstand)

#define CHAR(a) #a[0]


#define X(_,op) op_##op,

enum arith_token_type_t : uint8_t {
  _null = 0,
  _uint32_t,
  OPERATOR_X_LIST
  parent_open,
  parent_close,
};
#undef X


typedef struct{
  enum arith_token_type_t type;

  union{
    uint32_t _uint32_t;
  } data;

} arith_token_t;

typedef arith_token_t*  expr_t;

void print_tree(const expr_t);

expr_t parse_expr_from_str(const char*);

uint32_t eval_expr(const expr_t);
uint32_t eval_expr_n(const expr_t, uint16_t);



#define OPERATOR(name) (arith_token_t){.type = op_##name, .data={}}
#define NUMBER(val)    (arith_token_t){.type = _uint32_t, .data={._uint32_t=val}}

#define OPEN_PARENT()  (arith_token_t){.type = parent_open, .data={}}
#define CLOSE_PARENT()  (arith_token_t){.type = parent_close, .data={}}

#define NOP() (arith_token_t){}




#define MIN(a,b) ((a<b)? (a) : (b))
#define MAX(a,b) ((a>b)? (a) : (b))
#define CLAMP(x,a,b) ((x>b)? b : (x<a)? a : x) // aka MIN(b, MAX(x,a))
#define SWAP_XOR(a,b) {a^=b; b^=a; a^=b;}
#define SWAP_BUF(a,b) {typeof(a) buf=b; b=a; a=b;}
#define ARRLEN(arr) (sizeof(arr)/sizeof(*arr))







#ifdef DEBUG_
#define DEBUG(code) code
#else
#define DEBUG(code)
#endif




