#include "header.h"

/*  flags:
 *    -e : eval
 *    -f : read from file
 *    -s : read from stdin (shortcut for -f /dev/stdin)
 *
 *
 *
 *
 */

#define ERR(code, fmt, ...) do {fprintf(stderr, fmt, ##__VA_ARGS__); exit(code);} while(0)

typedef enum flag_t : unsigned char {
  None            = 0x00,
  Eval            = 0x01,
  Input_Is_File   = 0x02, //act as a boolean

  Err             = 0xff,
} flag_t;

FILE* in = {};

flag_t get_flag(const char* arg){
  if(*arg!='-') return None;

  if(!arg[1]) ERR(1, "expected flag name after '-'");

  flag_t flag={};

  for(int i=1; arg[i]; i++){
    switch(arg[i]){ // flag table
      case 'e': DEBUG(puts("flag : Eval")); if(flag&Eval) ERR(1, "how can you evaluate twice, we're to dumb to do that here"); flag|=Eval; continue;
      case 's': DEBUG(puts("flag : Input_Is_File + stdin")); if(in) freopen("/dev/stdin", "r", in); else in = fopen("/dev/stdin", "r"); // share return with the next case
      case 'f': DEBUG(puts("flag : Input_Is_File")); if(flag&Input_Is_File) ERR(1, "two files flags?"); flag |= Input_Is_File; continue;
      default: ERR(1,"wtf you mean '-%c' ?!", arg[1]);
    }
  }
  return flag;
}

void exit_callback(void){
  if(in) fclose(in);
}

static inline void do_the_thing(flag_t flag, const char* arg){
  if(flag&Eval){
    DEBUG(puts("this is a evaluation"));
    if(flag&Input_Is_File){
      DEBUG(puts("this is a file"));
      if(!in) ERR(1, "no files specified ;-;");
      char buf[256];
      if(!fgets(buf, 256, in)) ERR(1,"no contents in the file?");
      puts(buf);
      expr_t e = parse_expr_from_str(buf);
      printf("%i\n", eval_expr(e));
      free(e);
    } else{
      expr_t e = parse_expr_from_str(arg);
      printf("%i\n", eval_expr(e));
      free(e);
    }
  } else{
  }
}

int main(int argc, const char** argv){
  if(argc==1){ // default branch
    return 0;
  }
  atexit(exit_callback);
  char flag={};
  for(int i=1;i<argc;i++){
    char f = get_flag(argv[i]);
    if(f){
      flag|=f;
      continue;
    }

    do_the_thing(flag, argv[i]);

    flag=None;
  }
  if(flag) do_the_thing(flag, NULL);


  return 0;
}
