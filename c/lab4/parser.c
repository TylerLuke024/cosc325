#include "lexer.c"

// this is probably the right way to do this
// but let's do use the two data structure approach instead
typedef struct {
    int lineno;
    char* line;
} Line;
Line storedProgram[10]; // group the line numbers together with the line

// data structures needed for the interpreter
int lineno = 0;    // if this is equal to 0 then we should execute immediately
char* lines[1000];   // preallocate enough room for 1000 lines if we have more lines than that, oh well.
int linenos[1000];   // map the corresponding position in "lines" to the "line #" in this data structure
int lineindex = 0;   // keeps track of how many lines we have and where the next line should be stored

// here's our dirty symbol table
int symboltable[26];   // position 0 = "A", position 1 = "B", ect
int symboldefined[26]; // store a 1 in a given position if that "symbol" has been defined

void line();
void statement();
void expr_list();
void var_list();
int expression();
int term();
int factor();
void relop();

// hard coded swap of the positions applied to two different data structures
void swap(int j, int k) {
    // lets take care of the char* first
    char* tmpc = lines[j];
    lines[j] = lines[k];
    lines[k] = tmpc;

    // now lets take care of swapping the ints
    int tmpi = linenos[j];
    linenos[j] = linenos[k];
    linenos[k] = tmpi;
}

void sort() {
    int i = 1;
    while(i < lineindex) {
        int j = i;
        while (j > 0 && linenos[j-1] > linenos[j]) {
            swap(j, j-1);
            j = j - 1;
        }
        i = i + 1;
    }
}

// serach through linesnos and return index if found otherwise return -1 (if not found)
int search(int lineno) {
    for (int i = 0; i < lineindex; i++) {
        if (linenos[i] == lineno) {
            return i;
        }
    }
    return -1;
}

/******************************************************/
/* main driver */
int main()
{

    // prep the symbol table
    for (int i=0; i<26; i++) {
        symboldefined[i] = 0;
    }

  /* Open the input data file and process its contents */
  if ((in_fp = fopen("front.in", "r")) == NULL)
    printf("ERROR - cannot open front.in \n");
  else
  {
    getChar();
    do
    {
      lex();
      line();
    } while (nextToken != EOF);
  }
}

// lex() MUST be called before this function
void line() {
    if (nextToken == NUMBER) {
        lineno = atoi(lexeme); // takes ascii string and coverts it into a integer
        int previndex = search(lineno);
        if (previndex < 0) {
            linenos[lineindex] = lineno;
        }

        // take whatever is left in the rest of the line and store it for processing later!

        // consume the token by looking at the line number
        // and storing the statement that follows in the right place in our stored program
        // BUT NOT FOR THIS ASSIGNMENT
        // Call our special lex_endl() function to extract the rest of the line out of the file
        lex_endl(); // sets a global variable named rest_of_line that we need to store in 
                    // the right place in our lines datastructure;

        // allocate memory for the new line we just read in via lex_endl()
        // and then copy the line we just read into that new memory location
        if (previndex < 0) {
            lines[lineindex] = malloc(1000);
            strcpy(lines[lineindex], rest_of_line);
            printf("Storing line number %d at index %d\n", lineno, lineindex);
            lineindex++;
        } else {
            // we are overwriting an old line so just copy it over the psace that was already allocated for the old line
            strcpy(lines[previndex], rest_of_line);
            printf("Overwrite line number %d at index %d\n", lineno, previndex);
        }
        
    } else {
        statement(); // note that statement MUST have an extra call to lex()
    }
    if (nextToken != CR && nextToken != EOF) {
        printf("Expecting CR, but found: %d instead!\n", nextToken);
    } 

}

// lex() MUST have already been called before here
void statement() {
    switch(nextToken) {
        case PRINT:
            lex();
            expr_list();
            // unconditionally printf("\n");
            break;

        case IF:
            lex();
            expression(); // all expressions have an extra call to lex() because of term()
            relop(); // this always ends with an extra call to lex()
            expression();
            if (nextToken != THEN) {
                printf("error! expecting then found something else");
            }
            lex();
            statement();
            // we never need an extra call to lex() here 
            // because statement() ALWAYS has an extra call to lex()
            break;

        case GOTO:
            lex();
            expression();
            // no extra call to lex to look for the carriage return
            break;

        // keep going with more cases INPUT DOES NOT NEED THE EXTRA CALL TO LEX ... NEITHER DO THE ONES THAT ARE JUST KEYWORDS
        case INPUT:
            lex();
            var_list();
            break;

        case LET:
            lex();
            if (nextToken != VAR) {
                printf("Expecting IDENT but found: %d\n", nextToken);
                exit(1);
            }
            int pos = lexeme[0] - 'A';
            symboldefined[pos] = 1;
            lex();
            if (nextToken != EQUALS_OP) {
                printf("Expecting EQ but found: %d\n", nextToken);
                exit(1);
            }
            lex();
            symboltable[pos] = expression();

            // no extra call to lex() here because expression() will have already called lex() for us when it was looking for +, -, *, or /
            break;
        
        case GOSUB:
            lex();
            expression();

            // NO extra call to lex to look for the carriage return b/c expression() has an extra call to lex()
            break;
            
        case CLEAR:
            lineindex = 0;
            lex(); // this IS the extra call to lex() since nothing comes after these keywords
            break;
        case LIST:
            sort();
            for (int i=0; i<lineindex; i++) {
              printf("%d: %s\n", linenos[i], lines[i]);
            }
            lex(); // this IS the extra call to lex() since nothing comes after these keywords
            break;
        case RETURN:
        case RUN:
            sort();
            // BIG TODO HERE: update the lexer to take in a string instead of always reading from a file
            // ... DO THIS BY adding a FLAG variable to the lexer to indicate whether it should be grabbing the next token from a FILE
            // also add a function that will set a global variable 
        case END:
            lex(); // this IS the extra call to lex() since nothing comes after these keywords
            break;
    }
}

// makes an extra call to lex() to look for the comma
// lex has ALREADY been called before expr_list
void expr_list() {
    if (nextToken == STRING) {
        // extra call to lex() to look for the comma or carriage return after the string
        lex();
        printf("%s\t", lexeme);
        // do nothing else for this assignment
        // but in the next assignment you will need to print something!
    } else {
        printf("%s\t", expression());
        // expression ends with an extra call to lex() so we are already looking for the comma or carriage return by the time we get back here
    }
    while (nextToken == COMMA) {
        lex(); // move past the comma (i.e., consume the comma by looking for the next token after the comma)
        // next assignment: printf("\t");
        if (nextToken == STRING) {
            // extra call to lex() to look for the comma or carriage return after the string
            printf("%s\t", lexeme);
            // do nothing else for this assignment
            // but in the next assignment you will need to print something
        } else {
            printf("%s\t", expression());
            // no extra call to lex() here because expression() will have already called lex() for us when it was looking for +, -, *, or /
        }
        // there are only two valid tokens AT THIS SPOT
        if (nextToken != COMMA && nextToken != CR) {
            printf("Expecting COMMA or CR but found: %d\n", nextToken);
            exit(1);
        }
    }
    printf("\n");
}

void var_list() {
    if (nextToken != VAR) {
        printf("Expecting VAR but found: %d\n", nextToken);
    }
    else {
        lex();
        // do nothing else for this assignment
        // but in the next assignment you will need to print something
    }
    while (nextToken == COMMA) {
        lex();
        if (nextToken != VAR) {
            printf("Expecting VAR but found: %d\n", nextToken);
        }
        else {
            lex();
            // do nothing else for this assignment
            // but in the next assignment you will need to print something
        }
    }
}

int expression() {
    if(nextToken == ADD_OP || nextToken == SUB_OP) {
        lex(); // move past the leading + or - if it was there otherwise, the current nextToken is part of the term so no need to call lex()
    }
    int result = term();
    // no need to call lex() here because term() will have already called lex() for us when it was looking for * or /
    while (nextToken == ADD_OP || nextToken == SUB_OP) {
        lex(); // move past the + or -
        if (nextToken == ADD_OP)
            result += term();
        else
            result -= term();
        // remember, term() will have already called lex() for us when it was looking for * or / so no need to call it again here
    }
    // no need for extra call to lex() here because the while loop will have already called lex() for us when it was looking for + or -
    return result;
}

int term() {
    // you gotta do something here ... should be very similar to expression() but looking for * and / instead of + and -
    // note that term() will end up having an extra call to lex() at the end just like expression() does
    int result = factor();
    lex(); // look for mult op or div op
    while (nextToken == MULT_OP || nextToken == DIV_OP) {
        lex();
        if (nextToken == MULT_OP)
            result *= factor();
        else
            result /= factor();
        factor();
        lex();
    }
    return result;
}

int factor() {
    // look back at the grammar for all the possibilities for a factor ... you need if else if to handle identifiers, numbers, and parenthesized expressions
    // you should make very sure NOT to have an extra call to lex() here (finally!) because expression() and term() are the ones that need the extra calls to lex() to look for +, -, *, or /
    if (nextToken == VAR) {
        // look up the value in the symbol table error if not defined
        int pos = lexeme[0] - 'A';
        if (symboldefined[pos]==0) {
            printf("How? Undefined symbol: %c\n", lexeme[0]);
            exit(1);
        }
        return symboltable[pos];
    }
    else if (nextToken == NUMBER) {
        return atoi(lexeme);
    }
    else if (nextToken == LEFT_PAREN) {
        lex();
        int result = expression();
        if(nextToken != RIGHT_PAREN) {
            printf("Found %d but expecting RIGHT PAREN\n", nextToken);
        }
        return result;
    }
    // this should NEVER happen 
    printf("What? Expecting VAR, NUMBER, or LEFT_PAREN but found: %d\n", nextToken);
    exit(1);
    return -999999;
}

// this always has an extra call to lex()
void relop() {
    if (nextToken == LT_OP) {
        lex();
        if (nextToken == RT_OP || nextToken == EQUALS_OP) {
            lex();
        }
    }
    else if (nextToken == RT_OP) {
        lex();
        if (nextToken == LT_OP || nextToken == EQUALS_OP) {
            lex();
        }
    }
    else if (nextToken == EQUALS_OP) {
        lex();
    }
    else {
        printf("Expecting some valid REL_OP but found: %d\n", nextToken);
        exit(1);
    }
}