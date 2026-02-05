#include "lexer.c" // never supposed to add a c file into another c file (bad practice)

// data structures needed for the interpreter
int lineno = 0; // if this is equal to 0 htne we should execute immediatly
char* lines[10]; // preallocate enough room for 10 lines
int linenos[10]; 

void line();
void statement();
void expr_list();
void expression();
void relop();

/******************************************************/
/* main driver */
int main() {
   /* Open the input data file and process its contents */
   if  ((in_fp = fopen("front.in", "r")) == NULL)
     printf("ERROR - cannot open front.in \n");
   else  {
     getChar();
     do   {
       lex();
       line();
    }  while (nextToken != EOF);
  }
}

// lex() MUST be called before this function
void line(){
    if(nextToken == NUMBER) {
        lineno = atoi(lexeme);
        // take whatever is left in the rest of the oline and store it and process it later

        // consume the token by looking at the line number and storing the statement that follows in the right place
        // BUT NOT FOR THIS ASSIGNMENT
        // Call lex() to get the next token
        lex();
    } 
    statement(); // note that statement MUST have an extra call to lex()
    if (nextToken != CR) {
        printf("Expecting CR, but found: %d instead!\n", nextToken);
    }
}

// lex() must have already been called before here
void statement() { // keep going with more cases INPUT DOES NOT NEED THE EXTRA CALL TO LEX (RETURN and below dont need )
    switch(nextToken){
        case PRINT:
            lex();
            expre_list();
            break;
        case IF:
            lex();
            expression();
            lex();
            relop();
            lex();
            expression();
            statement();
            // we never need an extra call to lex() here
            // because statement() ALWAYS has an extra call to lex()
            break;
        case GOTO:
            lex();

            //extra call to lex to look for the character return
            lex();
            break;
        case INPUT:
            lex();

            break;
        case LET:
            lex();

            //extra call to lex to look for the character return
            lex();
            break;
        case GOSUB:
            lex();

            //extra call to lex to look for the character return
            lex();
            break;

        case RETURN:
        case CLEAR:
        case LIST:
        case RUN:
        case END:
            lex();
            break;
        
    }
}

// makes an extra call to lex() to look for the comma
// lex has already been called before expr_list
void expr_list() {
    if (nextToken == STRING) {
        if (lineno > 0) {
            printf("%s\n",lexeme);
        } else {
            expression();
        }
    }
    lex(); // extra call to look for the comma
    while (nextToken == COMMA) {
        if (nextToken == STRING) {
            if (lineno > 0) {
                printf("%s\n",lexeme);
            } else {
                expression();
            }
        }
        lex(); // extra call to look for the comma
        // there are only two valud tokens AT THIS SPOT
        if (nextToken != COMMA && nextToken != CR){
            printf("Expecting COMMA or CR but found: %d\n", nextToken);
            exit(-1);
        }
    }
}

void expression(){

}

void relop(){

}