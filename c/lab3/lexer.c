/* front.c - a lexical analyzer system for simple
             arithmetic expressions */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Global declarations */
/* Variables */
int  charClass;
char  lexeme [100];
char  nextChar;
int  lexLen;
int  token;
int  nextToken;
FILE *in_fp, *fopen();
/* Function declarations */
void  addChar();
void  getChar();
void  getNonBlank();
int  lex();

/* Character classes */ 
#define LETTER 0
#define DIGIT 1
#define QUOTE 2
#define UNKNOWN 99

/* Token codes */
#define STRING 8
#define NUMBER 10
#define FLOAT_LIT 9
#define IDENT 11
#define LT_OP 18
#define RT_OP 19
#define EQUALS_OP 20
#define ADD_OP 21
#define SUB_OP 22
#define MULT_OP 23
#define DIV_OP 24
#define LEFT_PAREN 25
#define RIGHT_PAREN 26
#define PRINT 30
#define IF 31
#define THEN 32
#define GOTO 33
#define INPUT 34
#define LET 35
#define GOSUB 36
#define RETURN 37
#define CLEAR 38
#define LIST 39
#define RUN 40
#define END 41
#define COMMA 45
#define CR 99

/*****************************************************/
/* lookup - a function to lookup operators and parentheses
            and return the token */

int  lookup(char  ch) {
  switch  (ch) {
    case  '(':
      addChar();
      nextToken = LEFT_PAREN;
      break;

    case  ')':
      addChar();
      nextToken = RIGHT_PAREN;
      break;

    case  '+':
      addChar();
      nextToken = ADD_OP;
      break;

    case  '-':
      addChar();
      nextToken = SUB_OP;
      break; 

    case  '*':
      addChar();
      nextToken = MULT_OP;
      break;

    case  '/':
      addChar();
      nextToken = DIV_OP;
      break;

    case '=':
      addChar();
      nextToken = EQUALS_OP;
      break;
    
    case '<':
      addChar();
      nextToken = LT_OP;
      break;

    case '>':
      addChar();
      nextToken = RT_OP;
      break;

    case ',':
      addChar();
      nextToken = COMMA;
      break;
    
    case '\n':
      addChar();
      nextToken = CR;
      break;

    default:
      addChar();
      printf("Unexpected token found: %c while working on the current lexeme: %s\n", nextChar, lexeme);
      exit(1);
      break;
  }
  return  nextToken;
}

/*****************************************************/
/* addChar - a function to add nextChar to lexeme */
void  addChar() {
  if  (lexLen <= 98) {
    lexeme[lexLen++] = nextChar;
    lexeme[lexLen] = 0;
  }
  else
    printf("Error - lexeme is too long \n");
}
/*****************************************************/
/* getChar - a function to get the next character of 
             input and determine its character class */
void getChar() {
  int c = getc(in_fp);
  if (c == EOF) {
    charClass = EOF;
    nextChar = 0;
  } else {
    nextChar = (char)c;
    if (isalpha((unsigned char)nextChar))
      charClass = LETTER;
    else if (isdigit((unsigned char)nextChar))
      charClass = DIGIT;
    else if(nextChar == '"'){
      charClass = QUOTE;
    }
    else
      charClass = UNKNOWN;
  }
}

/*****************************************************/
/* getNonBlank - a function to call getChar until it
                 returns a non-whitespace character */
void getNonBlank() {
  while  (isspace(nextChar) && nextChar != '\n')
    getChar();
}

// examines current lexeme and returns specific token or IDENT if its not a keyword (make one for REM)
int keywordLookup(){
  if(strcmp(lexeme, "PRINT")==0 || strcmp(lexeme, "PR")==0)
    return PRINT;
  else if(strcmp(lexeme, "IF")==0)
    return IF;
  else if(strcmp(lexeme, "THEN")==0)
    return THEN;
  else if(strcmp(lexeme, "GOTO")==0)
    return GOTO;
  else if(strcmp(lexeme, "INPUT")==0)
    return INPUT;
  else if (strcmp(lexeme,"LET")==0)
    return LET;
  else if (strcmp(lexeme,"GOSUB")==0)
    return GOSUB;
  else if (strcmp(lexeme,"RETURN")==0)
    return RETURN;
  else if (strcmp(lexeme,"CLEAR")==0)
    return CLEAR;
  else if (strcmp(lexeme,"LIST")==0)
    return LIST;
  else if (strcmp(lexeme,"RUN")==0)
    return RUN;
  else if (strcmp(lexeme,"END")==0)
    return END;
  else
    return IDENT;
    

}

/*****************************************************/
/* lex - a simple lexical analyzer for arithmetic 
         expressions */
// depends on charClass and nextChar already being set by the caller
int  lex() {
  lexLen = 0;
  getNonBlank();
  switch  (charClass) {
/* Parse identifiers */
    case  LETTER:
      addChar();
      getChar();
      while  (charClass == LETTER || charClass == DIGIT) {
        addChar();
        getChar();
      }
    nextToken = keywordLookup();
    break;

/* Parse integer literals */
    case  DIGIT:
      addChar();
      getChar();
      while  (charClass == DIGIT) {
       addChar();
       getChar();
   }

   if (nextChar == '.') {
    addChar();
    getChar();
    while  (charClass == DIGIT) {
      addChar();
      getChar();
      }
      nextToken = FLOAT_LIT;
      break;
   }
   nextToken = NUMBER;
   break;

/* Parentheses and operators */
    case  UNKNOWN:
      lookup(nextChar);
      getChar();
      break;

    case QUOTE:
      addChar();
      getChar();
      while  (charClass != QUOTE) {
        addChar();
        getChar();
      }
      addChar();
      getChar();
    nextToken = STRING;
    break;
    
/* EOF */
    case  EOF:
      nextToken = EOF;
      lexeme[0] = 'E';
      lexeme[1] = 'O';
      lexeme[2] = 'F';
      lexeme[3] = 0;
      break;
 } /* End of switch */
 printf("Next token is: %d, Next lexeme is %s\n", 
         nextToken, lexeme);
  return  nextToken;
} /* End of function lex */

