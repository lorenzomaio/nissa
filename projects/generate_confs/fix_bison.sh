#!/bin/bash

sed 's/YYERROR_VERBOSE_ARGS_MAXIMUM = [0-9]*/YYERROR_VERBOSE_ARGS_MAXIMUM = 100/
/switch (yycount).*/,/}/c\
#define YY_BASE_ERROR_STRING "syntax error, unexpected %s, expecting %s"\
#define YY_EXTEND_ERROR_STRING " or %s"\
char yyformatbuf[sizeof(YY_BASE_ERROR_STRING) + (sizeof(YY_EXTEND_ERROR_STRING)) * (YYERROR_VERBOSE_ARGS_MAXIMUM - 1)];\
strcpy(yyformatbuf, YY_BASE_ERROR_STRING);\
yyformat = yyformatbuf + sizeof(YY_BASE_ERROR_STRING) - 1;\
{\
  int yyi;\
  for (yyi = 1; yyi < yycount - 1; yyi++)\
  {\
    strcpy((char*)yyformat, YY_EXTEND_ERROR_STRING);\
    yyformat += sizeof(YY_EXTEND_ERROR_STRING) - 1;\
  }\
  yyformat = yyformatbuf;\
}' $1
