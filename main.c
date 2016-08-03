#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PREFIX "[bfc]"
#define MAX_CELL_INDEX 30000

char is_command(char);
void print_error (int, char*);
void print_syntax();

int main (int argc, char** argv) {
  enum BFC_VARS {BFC_NODEBUG = 1, BFC_UNALLOWED = 2};

  char global_vars = 0;

  if (argc < 2) {
    print_error(2, "Wrong syntax.");
    print_syntax();

    return 2;
  } 
  
  //----------------------------------------------------------------------------
  // Check if all commands are correct.

  int i = argc - 2;
  while(i--) {
    if (strcmp(argv[i + 2], "-nd") && strcmp(argv[i + 2], "--no-debug")){
      print_error(3, "Unknown command(s) used.");
      print_syntax();

      return 3;
    } else global_vars |= BFC_NODEBUG;
  } 

  //----------------------------------------------------------------------------
  

  FILE *f;
  f = fopen(argv[1], "r");

  if (f == NULL) {
    print_error(1, "Could not open/find specified file.");
    return 1;
  } else {
    char c, *fdata, *p;
    // l - loop level
    int i, fsize, l;
     
    fseek(f, 0, SEEK_END);
    fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    fdata = malloc(fsize);
    
    p = fdata;

    // Save all commands to fdata array.
    while ((c = fgetc(f)) != EOF) {
      if (is_command((char) c)) *(p++) = (char) c;
    }
    *p = '\0';

    //--------------------------------------------------------------------------
    // Debugging section (just find if all loops are correctly implemented).

    if (!(global_vars & BFC_NODEBUG)) {
      short lb = 0, rb = 0;
      p = fdata;
      while (*p) {
        if (*p == '[') ++lb;
        else if (*p == ']') ++rb;

        if (rb > lb) {
          global_vars |= BFC_UNALLOWED;
          printf("%s Error at command #%d - loose closing bracket.\n", 
            PREFIX, p - fdata + 1);
          break;
        }
        
        ++p;
      } 
     
      if (lb != rb) global_vars |= BFC_UNALLOWED; 
      rewind(f);
    }

    //--------------------------------------------------------------------------

    p = malloc(MAX_CELL_INDEX);
    memset(p, 0, MAX_CELL_INDEX);

    //--------------------------------------------------------------------------
    // Operate on each character (in other words - interpreting code!).

    if (global_vars & BFC_UNALLOWED) {
      print_error(4, "Debug error.");
      return 4;
    }

    for (i = 0; fdata[i] != 0; i++) {
      c = fdata[i];
      if (fdata[i] == '+') ++(*p);
      else if (fdata[i] == '-') --(*p);
      else if (fdata[i] == '<') --p;
      else if (fdata[i] == '>') ++p;
      else if (fdata[i] == '.') putchar(*p);
      else if (fdata[i] == ',') *p = getchar();
      else if (fdata[i] == '[') continue;
      else if (fdata[i] == ']' && *p) {
        l = 1;
        while (l) {
          c = fdata[--i];
          if (c == '[') --l;
          else if (c == ']') ++l;
        }
      }
    }

    free(fdata);
    // free(p);
    
    putchar('\n');
    //--------------------------------------------------------------------------
  }

  fclose(f);

  return 0;
}

char is_command(char c) {
  if (c == '+' || c == '-' || c == '<' || c == '>' || 
      c == '[' || c == ']' || c == '.' || c == ',')
    return 1;
  else return 0; 
}

void print_error(int err_num, char *message) {
  printf("%s Error E%d: %s\n", PREFIX, err_num, message);
}

void print_syntax() {
  printf("%s Syntax: bfc <filename> [--no-debug | -nd]\n", PREFIX);
}
