#ifndef TOKENIZER_H
#define TOKENIZER_H
enum state{
  word,
  decimal,
  octal,
  hexadecimal,
  floating_point,
  C_operator,
  error
};


struct TokenizerT_ {
  char *string;
  enum state current_state;
  int accept_state;
  char *start;
  char *curr;
  char *end;
};

typedef struct TokenizerT_ TokenizerT;

TokenizerT *TKCreate( char * ts );
void TKDestroy( TokenizerT * tk );

void isWord(TokenizerT * tk );
void isDecimal(TokenizerT * tk );
void isOctal(TokenizerT * tk );
void isHexadecimal(TokenizerT * tk );
void isFloat(TokenizerT * tk );
void isCToken(TokenizerT * tk );
void isMal(TokenizerT * tk );
void DestroySpace (TokenizerT * tk);
void printToken(TokenizerT *tk, char *next);

char *TKGetNextToken( TokenizerT * tk );

#endif
