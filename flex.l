%option reentrant bison-bridge bison-locations yylineno noyywrap noinput nounput

%{
 /* XOPEN for strdup */
 #define _XOPEN_SOURCE 600
 #include <limits.h>
 #include <stdlib.h>
 #include <string.h>
 #include <assert.h>
 #include "mylang.tab.h"
 #define YY_USER_ACTION yylloc->first_line = yylloc->last_line = yylineno;
 #define YY_EXIT_FAILURE ((void)yyscanner, EXIT_FAILURE)

%}

%%
"+"         {  return PLUS;     }
"-"         {  return MINUS;    }
"*"         {  return MUL;      }
"/"         {  return TRUE_DIV; }
"=<"        {  return LE;       }
">="        {  return GE;       }
"<"         {  return GT;       }
">"         {  return LT;       }
"("         {  return OP;       }
")"         {  return CP;       }
":"         {  return COLON;    }
","         {  return COMMA;    }
";"         {  return SEMICOL;  }
"=="        {  return EQ;       }
"="         {  return ASSIGN;   }
"end"       {  return END;      }
"not"       {  return NOT;      }

"function"  {  return FUNCTION; }
"var"       {  return VAR;      }
"do"        {  return DO;       }
"break"     {  return BREAK;    }
"while"     {  return WHILE;    }
"until"     {  return UNTIL;    }
"as"        {  return AS;       }
"wend"      {  return WEND;     }
"dim"       {  return DIM;      }
"loop"      {  return LOOP;     }


"array"     {  return ARRAY;   }
"bool"      {  return BOOLEAN; }
"byte"      {  return BYTE;    }
"uint"      {  return UINT;    }
"int"       {  return INT;     }
"ulong"     {  return ULONG;   }
"long"      {  return LONG;    }
"char"      {  return CHAR;    }
"string"    {  return STRING;  }

"if"        {  return IF;   }
"then"      {  return THEN; }
"else"      {  return ELSE; }

"false"                       {  yylval->value = strdup(yytext); return BOOL;  } 
"true"                        {  yylval->value = strdup(yytext); return BOOL;  } 
\"[^\"\\]*(?:\\.[^\"\\]*)*\"  {  yylval->value = strdup(yytext); return STR;   } 
'[^']'                        {  yylval->value = strdup(yytext); return CHR;   }
0[xX][0-9A-Fa-f]+             {  yylval->value = strdup(yytext); return HEX;   }
0[bB][01]+                    {  yylval->value = strdup(yytext); return BITS;  }
[0-9]+                        {  yylval->value = strdup(yytext); return DEC;   }
[a-zA-Z_][a-zA-Z_0-9]*        {  yylval->value = strdup(yytext); return IDENT; }

[ \t]       {  }
\n          {  }
.           { printf("critical error: mystery character\n"); return ERROR; }
%%
