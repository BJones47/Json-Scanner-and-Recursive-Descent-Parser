/*
* Name: Braxton Jones, 5007333716, As2
* Description: Using C this program will be a scanner and 
  a recursive decent parser
* Input: command line argument of file.
* Output: tokens and any errors found.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int EOT = 0; // end of file
const int STRING = 1; // sequence of chars
const int IDENT = 2; // identifier
const int COMMA = 3; // ','
const int COLON = 4; // ':'
const int LBRACE = 5; // '{'
const int RBRACE = 6; // '}'
const int LBRACK = 7; // '['
const int RBRACK = 8; // ']'
const int CONST = 9; // 'true', 'fase', 'null', numbers

// max size of strings
#define N 32

char *tokenNames[10] = {
"<EOF>", "STRING", "IDENT", "COMMA", "COLON",
"LBRACE", "RBRACE", "LBRACK", "RBRACK", "CONST",
};   

// token structure
struct Token {
int kind;
int lineNo;
char lexeme[N];
};

// global token list array
struct Token listOfTokens[500];
// token for getnexttoken()
struct Token nextToken;
// index for token list
int ind = 0;
// line number
int currLineNo = 1;
// file pointer
FILE* filePnt;
// string terminated error flag
int stringErr = 1; 
// parse error flag
int parseErr = 1;

// scanner functions
void match(int x);
void consume();
struct Token getNexttoken(char next);

// parser functions
void file();
void object();
void array();
void member();
void value();
void literal();
void name();
void boolean();


int main(int argc, char* argv[]) {
    struct Token holdToken;

    // lookahead char
    char lookHead;
    // char reader
    int ch;
    // scanner
    // read the file from command line
     filePnt = fopen(argv[1], "r");

    // while eof hasnt been reached read in ch and get the next token
    printf("\nScanning the file...'\n");
    while (ch != EOF) {
        ch = getc(filePnt);
        lookHead = ch;
        holdToken = getNexttoken(lookHead);
    }

    // print out token list
    // if there was a string error print out message
    // else print normal message
    if (stringErr == 0) {
        ind--;
        printf("Something went wrong! Unterminated STRING at line %d.\n\n"
        , currLineNo);
        printf("Printing tokens:");
    }   else {
        // output for scanner
        printf("Scanning done!\n\n");
        printf("List of tokens:");
    }
    
    for (int i = 1; i <= ind; i++) {            
        printf("\n<@%d,<%d,%s>='%s'>",  listOfTokens[i].lineNo, 
        listOfTokens[i].kind, tokenNames[listOfTokens[i].kind],
        listOfTokens[i].lexeme);
    }

    printf("\n\n");

    // parser 

    // if string error print out message and exit else parse
    if (stringErr == 0) {
        printf("Could not parse file.\n");
    }   else {
        // else parse file
        printf("Parsing file...\n");
        
        // reset index 
        ind = 1;
        nextToken = listOfTokens[ind];
        file();
        if (parseErr == 1) {
            printf("Parsing done!\n\n");
        }
    }
    exit(0);
}


// If lookahead token type matches x, consume and return; otherwise error
void match(int x) {
    if (nextToken.kind == x) {
        consume();
    }
    else {
        if (parseErr == 1) {
            fprintf(stderr, "Expecting '%s' but found '%s' on line %d.\n\n", 
            tokenNames[x], nextToken.lexeme, nextToken.lineNo);
        }
        // set parseErr flag to zero
        parseErr = 0;
    }
}

// Consume the next token in the input

void consume() {
    ++ind;
    nextToken = listOfTokens[ind];

}

// this function will take in a char and will find a pattern to make the next
// token until the eof has been found.
// input: this function takes in a char
// output: this file outputs a token that has been found in the file
struct Token getNexttoken(char next) {
    while (next != EOF && stringErr != 0) {
        switch (next) {
            // if space tab newline consume if newline inc line
            case ' ':
            case '\n':
            case '\t':
                if (next == '\n') {
                    currLineNo++;
                }
                next = getc(filePnt);
                continue;
            case '{':
                consume();
                listOfTokens[ind].kind = LBRACE;
                listOfTokens[ind].lineNo = currLineNo;
                listOfTokens[ind].lexeme[0] = next;
                return listOfTokens[ind];

            case '}':
                consume();
                listOfTokens[ind].kind = RBRACE;
                listOfTokens[ind].lineNo = currLineNo;
                listOfTokens[ind].lexeme[0] = next;
                return listOfTokens[ind];

            case ',':
                consume();
                listOfTokens[ind].kind = COMMA;
                listOfTokens[ind].lineNo = currLineNo;
                listOfTokens[ind].lexeme[0] = next;
                return listOfTokens[ind];
            
            case '[':
                consume();
                listOfTokens[ind].kind = LBRACK;
                listOfTokens[ind].lineNo = currLineNo;
                listOfTokens[ind].lexeme[0] = next;
                return listOfTokens[ind];

            case ']':
                consume();
                listOfTokens[ind].kind = RBRACK;
                listOfTokens[ind].lineNo = currLineNo;
                listOfTokens[ind].lexeme[0] = next;
                return listOfTokens[ind];

            case '"':
                consume();        
                // get the first character after the ""
                next = getc(filePnt);
                // counter for lexeme
                int cnt = 0;
                // while the string hasnt ended or eof been reached (stringerr)
                // put each char into the lexeme
                while (next != '"' && next != EOF) {
                    listOfTokens[ind].lexeme[cnt++] = next; 
                    next = getc(filePnt);
                }
                // if eof was found string was not terminated
                if (next == EOF) {
                    stringErr = 0;
                    ind--;
                    break;
                }
                listOfTokens[ind].kind = STRING;
                listOfTokens[ind].lineNo = currLineNo;
                return listOfTokens[ind];
            
            case ':':
                consume();
                // if the prior token to : is a string change to IDENT
                if (listOfTokens[ind-1].kind == STRING) {
                    listOfTokens[ind-1].kind = IDENT;
                }
                listOfTokens[ind].kind = COLON;
                listOfTokens[ind].lineNo = currLineNo;
                listOfTokens[ind].lexeme[0] = next;
                return listOfTokens[ind];

            default:
                consume();
                // default case = const 
                // get every character until whitespace or other identifiers

                // counter for lexeme index
                int j = 0;

                // whie in between capital, lowercase letters, and numbers read in
                while ((next >= 'A' && next <= 'Z') || (next >= 'a' && next <= 'z')
                        || (next >= '0' && next <= '9')) {
                    listOfTokens[ind].lexeme[j] = next;
                    next = getc(filePnt);
                    j++;
                }
                listOfTokens[ind].kind = CONST;
                listOfTokens[ind].lineNo = currLineNo;

                // if new line left over increment new line
                if (next == '\n') {
                    currLineNo++;
                    return listOfTokens[ind];
                }
                // if comma left over insert comma in next ind
                if (next == ',') {
                    consume();
                    listOfTokens[ind].kind = COMMA;
                    listOfTokens[ind].lineNo = currLineNo;
                    listOfTokens[ind].lexeme[0] = next;
                    return listOfTokens[ind-1];
                }

                printf("error line termiated at %d", currLineNo);
        }
        break;
    }
    // return eot token
    if (listOfTokens[ind].kind != EOT) {
        consume();
        listOfTokens[ind].kind = EOT;
        listOfTokens[ind].lineNo = currLineNo -1;
        listOfTokens[ind].lexeme[0] = 'E';
        listOfTokens[ind].lexeme[1] = 'O';
        listOfTokens[ind].lexeme[2] = 'F';
    }
    // return eot(eof) token
    return listOfTokens[ind];
}

// parser functions

// this function will start the recursive descent parser
// input: none
void file() {
    if (nextToken.kind == LBRACE)
        object();
    else if (nextToken.kind == LBRACK)
        array();
    else
        fprintf(stderr, "Error in file() no L Brace or Bracket");
        parseErr = 0;
}

// this function will recursively call another depending on the token
// input: none
void object() {
    if (nextToken.kind == LBRACE) {
        match(LBRACE);
        if (nextToken.kind == RBRACE) {
            match(RBRACE);
        } else {
            do {
                 if (nextToken.kind == COMMA) {
                    match(COMMA);
                }
                member(); // calling value
            } while (nextToken.kind == COMMA);
        }
    }
    match(RBRACE);
}

// this function will recursively call another depending on the token
// input: none
void array() {

    if (nextToken.kind == LBRACK) {
        match(LBRACK);
        if (nextToken.kind == RBRACK) {
            match(RBRACK);
        } else {
            do {
                if (nextToken.kind == COMMA) {
                    match(COMMA);
                }
                value(); // calling value

          } while (nextToken.kind == COMMA); 
        }
    }
    match(RBRACK);
}

// this function will recursively call another depending on the token
// input: none
void member() {
    if (nextToken.kind == IDENT || nextToken.kind == STRING) {
        name();
        match(COLON); // parse colon
        value();
    }   else {
        fprintf(stderr, "Error in member() no IDENT or STRING");
        parseErr = 0;
    }
}

// this function will recursively call another depending on the token
// input: none
void value() {

    if (nextToken.kind == LBRACE) {
        object(); // calling object recursively
    }
    else if (nextToken.kind == LBRACK) {
        array(); // calling array recursively
    }
    else if (nextToken.kind == CONST || 
             nextToken.kind == STRING) {
        literal(); // cllin literal recursively
    }
    else {
        fprintf(stderr, "Error in value() no LBRACE, LBRACK, CONST OR STRING");
        parseErr = 0;
    }

}

// this function will recursively call another depending on the token
// input: none
void literal() {
    // is int
    if (nextToken.lexeme[0] >= '0' && nextToken.lexeme[0] <= '9') {
        match(CONST);
    }   else if (nextToken.kind == STRING) {
        // is string
        match(STRING);

    }   else if (strcmp(nextToken.lexeme, "true") == 0 || 
                 strcmp(nextToken.lexeme, "false") == 0 ) {
        // is boolean
        boolean();
    }   else if (strcmp(nextToken.lexeme, "null") == 0) {
        // null is const
        match(CONST);
    }   else {
        fprintf(stderr, "Error in literal() no CONST found");
        parseErr = 0;
    }
}

// this function will recursively call another depending on the token
// input: none
void name() {
    if (nextToken.kind == STRING) {
        match(STRING);
    }   else if (nextToken.kind == IDENT) {
        match(IDENT);
    }  
}

// this function will recursively call another depending on the token
// input: none
void boolean() {
    if (strcmp(nextToken.lexeme, "true") == 0 || 
        strcmp(nextToken.lexeme, "false") == 0) {
        match(CONST);
    }   else {
        fprintf(stderr, "Error in boolean() no true or false values");
        parseErr = 0;
    }
}