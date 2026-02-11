#include "header.h"

#define IS_DIGIT(c) (c>='0'  && c<='9')

void print_token(arith_token_t t){
#define X(char, name) case op_##name: putchar(char); break;

  switch(t.type){
    case _null: printf("(null)"); break;

    case _uint32_t:
      printf("%u", t.data._uint32_t);
      break;

    OPERATOR_X_LIST;

    default: printf("(udf)"); break;
  }

#undef X
}

void print_tree(const expr_t e){
  for(expr_t t=e;t->type;t++){ print_token(*t); putchar(' ');}
}


static inline arith_token_t* tokenize(const char* str, uint16_t*const n){

  arith_token_t* tokens;
  {
    uint16_t str_l = 0;
    for(; str[str_l] ;str_l++);

    str_l++; // the FIX use this to ensure place for the NOP() at the end

    tokens = malloc(str_l*sizeof(*tokens));
  }

  uint16_t tokens_c={};
  DEBUG(puts(str););
#define PUSH(a) tokens[tokens_c++]= a

  for(const char*c=str; *c; c++){
    DEBUG(puts(c););
    if(IS_DIGIT(*c)){
      uint32_t buffer=*c -'0';
      if(!IS_DIGIT(c[1])) goto num_end;
      c++;
      for(;IS_DIGIT(*c); c++){
        buffer*=10;
        buffer+= *c -'0';
      }
      c--;
      num_end:

      tokens[tokens_c++]=(typeof(*tokens)){.type=_uint32_t, .data={._uint32_t=buffer}};
      continue;
    }
#define X(char, name) case char: tokens[tokens_c++]=(typeof(*tokens)){.type=op_##name, .data={}}; continue;

    switch(*c){

      OPERATOR_X_LIST;

      case ' ': continue;
      case '\n': break;

      case '(': PUSH(OPEN_PARENT()); continue;
      case ')': PUSH(CLOSE_PARENT()); continue;

      default: printf("unknown char %c(%hhu)\n", *c, *c);
      break;
    }

#undef X

#undef PUSH
  }


  tokens[tokens_c++]=NOP(); //FIX: this somehow work, but it just get beside the issue, details in 'parse_expr_from_str'
  *n = tokens_c;
  return tokens;
}

static inline uint16_t prio(arith_token_t t){
  switch(t.type){

    case op_amperstand: case op_pipe: case op_circumflex: return 3;

    case op_asterisk: case op_slash: return 2;

    case op_plus: case op_minus: return 1;

    case _null:
    case _uint32_t:
    default: return ~(uint16_t)0;
  }
}

static inline uint16_t find_min_prio(arith_token_t* t, uint16_t b, uint16_t e){
  uint16_t bracket_counter = 0;
  uint16_t min = prio(t[b]), min_i=b;
  for(uint16_t i=b;i<e;i++){
    if(t[i].type==parent_open) bracket_counter+=4;
    uint16_t p = prio(t[i]) + bracket_counter;
    if(min>p){
      min = p;
      min_i = i;
    }
  }
  return min_i;
}


expr_t parse_expr_from_str(const char* str){ // WARNING: this does work only for prefixed operators, so if function calls where to be implemented, a disjunction must be done between infixed and prefixed operators/functions
  uint16_t n;
  uint16_t stack_c=2;
  arith_token_t* tokens = tokenize(str, &n);
  if(!(tokens && n)) return (expr_t){};
  DEBUG(
      printf("parsing [ "); for(uint16_t i=0; i<n;i++){print_token(tokens[i]); putchar(' ');} printf("] of size %hu\n", n);
    );

  // issue of disparition between here and the return
  //   -> the token loss is the last of the list
  // ugly solution on the fix in 'tokenize'
  //  -> push a useless NOP() on the back so the loss is futile

#define PUSH(a) stack[stack_c++]=a
#define POP()   stack[--stack_c]
  uint16_t* stack = malloc(2*(n)*sizeof(uint16_t));
  stack[0]=0;
  stack[1]=n-1;
  expr_t ret = {malloc(n*sizeof(*tokens))};
  uint16_t ret_ptr = 0;
  while(stack_c){
    uint16_t end = POP(),
             begin = POP();


    if(begin == end) continue;
    uint16_t max_i =find_min_prio(tokens, begin, end);

    ret[ret_ptr++]=tokens[max_i];


    // NOTE: operation priority: right to left
    // left and right are switched from the array order because some operators does not commutes

    PUSH(max_i+1);  // begin right
    PUSH(end);      // end right
    PUSH(begin);    // begin left
    PUSH(max_i);    // end left


  }

#undef POP
#undef PUSH
  free(stack);
  free(tokens);

  ret[ret_ptr]=NOP();

  return ret;
}
