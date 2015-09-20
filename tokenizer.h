#ifndef TOKENIZER_H
#define TOKENIZER_H
enum state{
  zero,
  word,
  decimal,
  octal,
  hexadecimal,
  floating_point,
  error,
  //COperators
  leftbrace,
  rightbrace,
  leftbracket,
  rightbracket,
  structuremember,
  structurepointer,
  multiply,
  bitwiseand,
  leftparen,
  negate,
  onescomplement,
  inc,
  dec,
  rightparen,
  divide,
  modulo,
  add,
  subtract,
  shiftright,
  shiftleft,
  lessthan,
  greaterthan,
  lessorequal,
  greaterorequal,
  equals,
  notequals,
  bitwiseexclusiveor,
  bitwiseor,
  logicaland,
  logicalor,
  trueop,
  falseop,
  assignmentoperator,
  plusequals,
  minusequals,
  multiplyequals,
  divideequals,
  moduloequals,
  shiftrightequals,
  shiftleftequals,
  bitwiseandequals,
  bitwiseexclusiveorequals,
  bitwiseorequals,
  comma,

  c_keyword,
  string
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

void isMal(TokenizerT * tk );
void isIncomplete(TokenizerT * tk);
void isWord(TokenizerT * tk );
void isDecimal(TokenizerT * tk );
void isOctal(TokenizerT * tk );
void isHexadecimal(TokenizerT * tk );
void isFloat(TokenizerT * tk );
void isFloatE(TokenizerT * tk );
void isCToken(TokenizerT * tk );
void isKeyword(TokenizerT *tk);
int isSingleQuote(TokenizerT *tk);
int isDoubleQuote(TokenizerT *tk);
int isDelim(char ch);
int isEscape(char ch);

void printToken(TokenizerT *tk, char *next);

char *TKGetNextToken( TokenizerT * tk );

#endif
