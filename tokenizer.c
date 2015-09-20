/*
 * tokenizer.c
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "tokenizer.h"

 const char *keyword_list[32] = {
    "auto",
    "break",
    "case",
    "char", 
    "const", 
    "continue", 
    "default", 
    "do", 
    "double",
    "else",
    "enum",
    "extern",
    "float",
    "for",
    "goto",
    "if",
    "int",
    "long",
    "register",
    "return",
    "short",
    "signed",
    "sizeof",
    "static",
    "struct",
    "switch",
    "typedef",
    "union",
    "unsigned",
    "void",
    "volatile",
    "while"
  };

//creates a tokenizer object containing the input string with a default state of error
TokenizerT *TKCreate( char * ts ) {
  if(ts==NULL){
    return NULL;
  }

  TokenizerT *new_tokenizer =(TokenizerT*)malloc(sizeof(TokenizerT));

  new_tokenizer->string = (char*)malloc(strlen(ts)+1 * sizeof(char));
  strcpy(new_tokenizer->string,ts);

  new_tokenizer->current_state = error;

  new_tokenizer->start = new_tokenizer->string;
  new_tokenizer->curr = new_tokenizer->string;
  new_tokenizer->end = new_tokenizer->string;
  
  new_tokenizer->accept_state = 0;

  return new_tokenizer;
}

void TKDestroy( TokenizerT * tk ) {
  free(tk->string);
  free(tk);
}

//handles malformed tokens. 
void isMal(TokenizerT * tk ){
  //advance the pointers past the malformed token and set error state
  while(!isDelim(*(tk->curr+1))){
    tk->curr++;
  }
  tk->current_state = error;
  tk->curr++;
  tk->start = tk->curr;
  tk->end = tk->curr;
  printf("Malformed Token, Moving to Next\n");
  return;
}

//boolean check for a valid delimiter 
//(whitespace, end of string, COperators, and escape characters) 
int isDelim(char ch){
  if(isspace(ch)|| ch == '\0' ) return 1;
  //tokens are delimited by Coperators
  if(
     ch == 33
     ||ch == 37
     ||ch == 38
     ||((ch>=40)&&(ch<= 47))
     ||ch == 58
     ||((ch>=60)&&(ch<=63))
     ||ch == 91
     ||ch == 93
     ||ch == 94
     ||ch == 124
     ||ch ==126
     ){return 1;}
  //tokens are delimited by escape characters
  if(isEscape(ch)) return 1;
  return 0;
}

//Recursively checks if the token is a word
void isWord(TokenizerT * tk ){
  if(isDelim(*(tk->curr+1))){
    tk->current_state = word;
    (tk->end) = (tk->curr);
    tk->curr++;
    return;
  }
  else if( isalnum(*((tk->curr)+1)) ){
    (tk->curr)++;
    isWord(tk);
  }
  else{
    tk->curr++;
    isMal(tk);
  }
}

//Recursively checks if token is a Decimal, passing to isFloat or isFloatE is as necessary 
void isDecimal(TokenizerT * tk ){
  if(isDelim(*(tk->curr+1)) && *(tk->curr+1)!= '.'){
    tk->current_state = decimal;
    (tk->end) = (tk->curr);
    tk->curr++;
    return;
  }
  else if( isdigit( *((tk->curr)+1) )){
    tk->curr++;
    isDecimal(tk);
  }
  else if( *((tk->curr)+1) == '.' ){
    tk->curr++;
    isFloat(tk);
  }
  else if( *((tk->curr)+1) == 'E' || *((tk->curr)+1) == 'e' ){
    tk->curr++;
    isFloatE(tk);
  }
  else{
    tk->curr++;
    isMal(tk);
  }
}

//Recursively checks if token is a OcatalDecimal which cannot become a float 
void isOctalDecimal(TokenizerT * tk ){
  if(isDelim(*(tk->curr+1))){
    tk->current_state = decimal;
    (tk->end) = (tk->curr);
    tk->curr++;
    return;
  }
  else if( isdigit( *((tk->curr)+1) )){
    tk->curr++;
    isOctalDecimal(tk);
  }
  else{
    tk->curr++;
    isMal(tk);
  }
}
//Recursively checks if the token is an Octal, passing to is Decimal as necessary
void isOctal(TokenizerT * tk ){
  if(isDelim(*(tk->curr+1))){
    tk->current_state = octal;
    (tk->end) = (tk->curr);
    tk->curr++;
    return;
  }
  else if( isdigit( *((tk->curr)+1) )){
    switch( *((tk->curr)+1) ){
    case '8':
      isOctalDecimal(tk);
      break;
    case '9':
      isOctalDecimal(tk);
      break;
    default:
      (tk->curr)++;
      isOctal(tk);
      break;     
    }
  }
  else{
    (tk->curr)++;
    isMal(tk);
  }
}

//Recursively checks if the token follows hexadecimal format.
void isHexadecimal(TokenizerT * tk ){
  if(isDelim(*(tk->curr+1))){
    if( *(tk->curr)=='x' || *(tk->curr)=='X' ){
      isMal(tk);
      return;
    }
    tk->current_state = hexadecimal;
    (tk->end) = (tk->curr);
    tk->curr++;
    return;
  }
  else if( isxdigit( *((tk->curr)+1) )){
    tk->curr++;
    isHexadecimal(tk);   
  }
  else{
    (tk->curr)++;
    isMal(tk);
  }  
}

//Recursively checks if token is in float format with no eE, passing to isFloatE as neccessary
void isFloat(TokenizerT * tk ){
  if(isDelim(*(tk->curr+1))){
    if( *(tk->curr)=='.' ){
      isMal(tk);
      return;
    }
    tk->current_state = floating_point;
    (tk->end) = (tk->curr);
    tk->curr++;
    return;
  }
  else if( isdigit( *((tk->curr)+1) )){
    tk->curr++;
    isFloat(tk);
  }
  else if( *((tk->curr)+1) == 'e' || *((tk->curr)+1) =='E' ){
    tk->curr++;
    isFloatE(tk);
  }
  else{
    (tk->curr)++;
    isMal(tk);
  }  
}

//recursively checks if token is in valid format with an eE
void isFloatE(TokenizerT * tk ){  
  if(isDelim(*(tk->curr+1))&&(*(tk->curr+1)!='+'&& *(tk->curr+1)!='-') ){
    if( *(tk->curr)=='e' ||  *(tk->curr)=='E' || *(tk->curr) == '+' || *(tk->curr) == '-' ){
      isMal(tk);
      return;
    }
    tk->current_state = floating_point;
    (tk->end) = (tk->curr);
    tk->curr++;
    return;
  }
  else if( isdigit( *((tk->curr)+1) )){
    tk->curr++;
    isFloatE(tk);
  }
  else if( *((tk->curr)+1) == '+' || *((tk->curr)+1) == '-' ){
    if( *(tk->curr) == 'e' || *(tk->curr) == 'E' ){
      tk->curr++;
      isFloatE(tk);
    }
    else{
      (tk->curr)++;
      isMal(tk);
    }
  }
  else{
    (tk->curr)++;
    isMal(tk);
  }    
}

//checks if the token is a valid c token, passing the token to isSingleQuote or isDoubleQuote and necessary.
void isCToken(TokenizerT * tk ){
  switch( *(tk->curr) ){
   case '-':
      if( (*((tk->curr)+1) )=='>' ){
	tk->current_state = structurepointer;
	tk->curr+=2;
	tk->end = tk->curr-1;
      }
      else if( (*((tk->curr)+1) ) == '-' ){
	      tk->current_state = dec;
	      tk->curr+=2;
	      tk->end = tk->curr-1;
	    }
      else if( (*((tk->curr)+1) )== '=' ){
	      tk->current_state = minusequals;
	      tk->curr+=2;
	      tk->end = tk->curr-1;
	    }
      else{
	      tk->current_state = subtract;
	      tk->curr++;
	    }
  	  break;
    case '+':
      if( *((tk->curr)+1) =='=' ){
              tk->current_state = plusequals;
	      tk->curr+=2;
	      tk->end = tk->curr-1;
	    }
	    else if( *((tk->curr)+1)=='+' ){
	      tk->current_state = inc;
	      tk->curr+=2;
	      tk->end = tk->curr-1;
	    }
	    else{
	      tk->current_state = add;
	      tk->curr++;
	    }
	    break;
    case '=':
      if( *((tk->curr)+1) =='=' ){
	    tk->current_state = equals;
	      tk->curr+=2;
	      tk->end = tk->curr-1;
	    }
      else{
	      tk->current_state = assignmentoperator;
	      tk->curr++;
	    }
      break;
    case '*':
      if( *((tk->curr)+1) == '=' ){
	      tk->current_state = multiplyequals;
	      tk->curr+=2;
	      tk->end = tk->curr-1;
	    }
      else{
    	tk->current_state = multiply;
    	tk->curr++;
      }	
      break;
      
    case '%':
      if( *((tk->curr)+1)=='=' ){
	      tk->current_state = moduloequals;
	      tk->curr+=2;
	      tk->end = tk->curr-1;
	    }
      else{
	      tk->current_state = modulo;
	      tk->curr++;
      }
	    break;
    case '>':
      if( *((tk->curr)+1) =='=' ){
	      tk->current_state = greaterorequal;
	      tk->curr+=2;
	      tk->end = tk->curr-1;
	    }
      else if(*((tk->curr)+1) =='>' ){
	      if( *((tk->curr)+2) =='=' ){
	        tk->current_state = shiftrightequals;
	        tk->curr+=3;
	        tk->end = tk->curr-1;
	      }
        else{
	        tk->current_state = shiftright;
	        tk->curr+=2;
	        tk->end = tk->curr-1;
	      }
      }
      else{
        tk->current_state = greaterthan;
        tk->curr++;
      }
	    break;
    case '<':
      if(*((tk->curr)+1) =='=' ){
	      tk->current_state = lessorequal;
	      tk->curr+=2;
	      tk->end = tk->curr-1;
      }
      else if( *((tk->curr)+1) =='<' ){
	      if( *((tk->curr)+2) =='=' ){
	        tk->current_state = shiftleftequals;
	        tk->curr+=3;
	        tk->end = tk->curr-1;
	      }
	      else{
	        tk->current_state = shiftleft;
	        tk->curr+=2;
	        tk->end = tk->curr-1;
	      }
      }
      else{
	      tk->current_state = lessthan;
	      tk->curr++;
      }
	    break;
    case '/':
	    if(*((tk->curr)+1) == '='){
	      tk->current_state = divideequals;
	      tk->curr+=2;
	      tk->end = tk->curr-1;
	    }
	    else{
	      tk->current_state = divide;
	      tk->curr++;
	    } 
  	  break;
    case '(':
	    tk->current_state = leftparen;
	    tk->curr++;
	    break;
    case ')':
	    tk->current_state = rightparen;
	    tk->curr++;
    	break;
    case '.':
	    tk->current_state = structuremember;
	    tk->curr++;
    	break;
    case '&':
      if( *((tk->curr)+1) == '='){
	      tk->current_state = bitwiseandequals;
	      tk->curr+=2;
	      tk->end = tk->curr-1;
	    }
      else if( *((tk->curr)+1) == '&'){
	      tk->current_state = logicaland;
	      tk->curr+=2;
	      tk->end = tk->curr-1;
	    }
      else{
	      tk->current_state = bitwiseand;
	      tk->curr++;
      } 
	    break;
    case '!':
      if( *((tk->curr)+1) == '='){
	      tk->current_state = notequals;
	      tk->curr+=2;
	      tk->end = tk->curr-1;
       }
       else{
	      tk->current_state = negate;
	      tk->curr++;
       } 
    	break;
    case '~':
	    tk->current_state = onescomplement;
	    tk->curr++;
    	break;
    case '^':
      if(*((tk->curr)+1) == '='){
	      tk->current_state = bitwiseexclusiveorequals;
	      tk->curr+=2;
	      tk->end = tk->curr-1;
	    }
	    else{
	      tk->current_state = bitwiseexclusiveor;
	      tk->curr++;
	    } 
	    break;
    case '[':
	    tk->current_state = leftbracket;
	    tk->curr++;
    	break;
    case ']':
	    tk->current_state = rightbracket;
	    tk->curr++;
	    break;
    case '?':
	    tk->current_state = trueop;
	    tk->curr++;
	    break;
    case ':':
	    tk->current_state = falseop;
	    tk->curr++;
	    break;
    case ',':
	    tk->current_state = comma;
	    tk->curr++;
	    break;
    case '|':
       if(*((tk->curr)+1) == '|'){
	      tk->current_state = logicalor;
	      tk->curr+=2;
	      tk->end = tk->curr-1;
        }
      else if(*((tk->curr)+1) == '='){
	      tk->current_state = bitwiseorequals;
	      tk->curr+=2;
	      tk->end = tk->curr-1;
        }

       else{
	      tk->current_state = bitwiseor;
	      tk->curr++;
	    } 
    	break;
    case '\'':
      if(!isSingleQuote(tk)){
	isMal(tk);
      }
      break;
    case '\"':
      if(!isDoubleQuote(tk)){
	isMal(tk);
      }
      break;
    default:
	    isMal(tk);
    	break;
    }
  return;
}

//Checks if token is a valid C keyword, using the valid keyword_list array.
//If not, passes the token to isWord to be treated as a word
void isKeyword(TokenizerT *tk){
  int i, string_len;
  for(i = 0; i < 32; i++){
    string_len = strlen( keyword_list[i] );
    if( (strncmp(tk->curr, keyword_list[i], string_len) == 0) && isDelim(*(tk->curr+string_len)) ){
        tk->current_state = c_keyword;
        tk->end = tk->start + (string_len-1);
	tk->curr += string_len;        
	return;
     }
  }
  isWord(tk);
  return;
}

//Checks if token is a valid single-quotes string
int isSingleQuote(TokenizerT *tk){
  tk->curr++;
  while(*(tk->curr) != '\0'){
    if( (*(tk->curr) == '\'') ){
      tk->curr++;
      tk->end = tk->curr-1; 
      tk->current_state = string;
      return 1;
    }
     tk->curr++;
  }
  tk->curr = tk->start;
  return 0;
}

//Checks if token is a balid double-quoted string
int isDoubleQuote(TokenizerT *tk){
  tk->curr++;
  while(*(tk->curr) != '\0'){
    if( (*(tk->curr) == '\"') ){
      tk->curr++;
      tk->end = tk->curr-1; 
      tk->current_state = string;
      return 1;
    }
     tk->curr++;
  }
  tk->curr = tk->start;
  return 0;
}

//Checks token is a valid multiline comment, setting start and end positions to skip accordingly
int isMultiComment(TokenizerT *tk){
  tk->curr+=2;
  while(*(tk->curr) != '\0'){
    if( (*(tk->curr) == '*') && (*(tk->curr+1) == '/') ){
      tk->curr += 2;
      tk->start = tk->curr;
      tk->end = tk->curr; 
      return 1;
    }
     tk->curr++;
  }
  tk->curr = tk->start; 
  return 0;
}

//Checks if token is a valid singleline C comment, setting the start and end positions to skip accordingly
int isSingleComment(TokenizerT *tk){
  tk->curr+=2;
  while(*(tk->curr) != '\0'){
    if( (*(tk->curr) == '\n') ){
      tk->curr++;
      tk->start = tk->curr;
      tk->end = tk->curr; 
      return 1;
    }
     tk->curr++;
  }
  tk->start = tk->curr;
  tk->end = tk->curr;
  return 0;  
}

//checks if the character is a valid escape character
int isEscape(char ch){
  if(((ch >= 0) && (ch <= 31))|| ch >=127) return 1;
  return 0;
}

//Prints the state and the token using switch cases to determine the state to print
void printToken(TokenizerT *tk, char *next){
  switch( (tk->current_state) ){
  case 0:
    printf("zero");
    break;
  case 1:
    printf("word");
    break;
  case 2:
    printf("decimal");
    break;
  case 3:
    printf("octal");
    break;
  case 4:
    printf("hexadecimal");
    break;
  case 5:
    printf("float");
    break;
  //CTokens
  case 7:
    printf("leftbrace");
    break;
  case 8:
    printf("rightbrace");
    break;
  case 9:
    printf("leftbracket");
    break;
  case 10:
    printf("rightbracket");
    break;
  case 11:
    printf("structuremember");
    break;
  case 12:
    printf("structurepointer");
    break;
  case 13:
    printf("multiply");
    break;
  case 14:
    printf("bitwiseand");
    break;
  case 15:
    printf("left parentheses");
    break;
  case 16:
    printf("negate");
    break;
  case 17:
    printf("onescomplement");
    break;
  case 18:
    printf("inc");
    break;
  case 19:
    printf("dec");
    break;
  case 20:
    printf("right parentheses");
    break;
  case 21:
    printf("divide");
    break;
  case 22:
    printf("modulo");
    break;
  case 23:
    printf("add");
    break;
  case 24:
    printf("subtract");
    break;
  case 25:
    printf("shiftright");
    break;
  case 26:
    printf("shiftleft");
    break;
  case 27:
    printf("less than");
    break;
  case 28:
    printf("greater than");
    break;
  case 29:
    printf("less than or equal");
    break;
  case 30:
    printf("greater than or equal");
    break;
  case 31:
    printf("equals");
    break;
  case 32:
    printf("not equals");
    break;
  case 33:
    printf("bitwise exclusive or");
    break;
  case 34:
    printf("bitwise or");
    break;
  case 35:
    printf("logical and");
    break;
  case 36:
    printf("logical or");
    break;
  case 37:
    printf("true conditional");
    break;
  case 38:
    printf("false conditional");
    break;
  case 39:
    printf("assignment operator");
    break;
  case 40:
    printf("plus equals");
    break;
  case 41:
    printf("minus equals");
    break;
  case 42:
    printf("multiply equals");
    break;
  case 43:
    printf("divide equals");
    break;
  case 44:
    printf("modulo equals");
    break;
  case 45:
    printf("shift right equals");
    break;
  case 46:
    printf("shift left equals");
    break;
  case 47:
    printf("bitwise and equals");
    break;
  case 48:
    printf("bitwise exclusive or equals");
    break;
  case 49:
    printf("bitwise or equals");
    break;
  case 50:
    printf("comma");
    break;

  // keywords
  case 51:
    printf("C Keyword");
    break;
  case 52:
    printf("string");
    break;
  default:
    printf("Unknown State");
    break;
  }

  printf(" \"%s\"\n", next);
}

//Our Main function. 
//This function pulls out the next token in the input string and determines its state.
//At the end of this function, the state and the token itself is printed.
//The function returns the string if we find a valid token, or null otherwise.

char *TKGetNextToken( TokenizerT * tk ) {
  //skips past blank space
  if( isspace(*(tk->curr)) ){
    tk->curr++;
    tk->start = tk->curr;
    tk->end = tk->curr;
    return NULL;
  }   

  // standard case functions here
  if(isdigit(*(tk->curr))){
    if(*(tk->curr) == '0'){
      //check if valid delimiator exists after the zero (zero-state)
      if(isDelim(*(tk->curr+1)) && *(tk->curr+1) != '.'){
	      tk->current_state = zero;
	      tk->curr++;
      } 
      else{
	      switch( *((tk->curr) + 1) ){
	        case '.':
		        if(isDelim(*(tk->curr+2))){
		          tk->current_state = zero;
              tk->curr++;
		        }
		        else{
		          tk->curr++;
		          isFloat(tk);
		        }
	          break;
	        case 'x':
	          tk->curr++;
	          isHexadecimal(tk);
	          break;
	        case 'X':
	          tk->curr++;
	          isHexadecimal(tk);
	          break;
	        default :
            if(isDelim(*(tk->curr + 1))){
	            tk->curr++;
              tk->current_state = zero;
            }
            else{
	            isOctal(tk);
            }
	    break;            
	     }
     }
    }
    else{
      isDecimal(tk);
    }
  }

  else if(ispunct(*(tk->start))){
    if(*(tk->curr)=='/' && *(tk->curr+1)=='*'){
      if(isMultiComment(tk)) return NULL;
    }
    else if(*(tk->curr)=='/'&&*(tk->curr+1)=='/'){
      isSingleComment(tk);
      return NULL;
    }
    isCToken(tk);
  }
  //tokens that start with alphabetic characters (word, keyword) are dealt with here
  else if(isalpha(*(tk->start))){
    isKeyword(tk);
    }
  else{
    if(isEscape(*(tk->curr))){ 
      printf("[%#x]\n", *(tk->curr));
    }
    else{ 
      printf("something is very wrong\n");
    }
    tk->curr++;
    tk->start = tk->curr;
    tk->end = tk->curr;
    return NULL;
  }

  if(tk->current_state == error){ 
    return NULL;
  }
  /*
    Have to calloc because reusing same memory so often sometimes gives us dirty memory, 
    i.e. printouts that are wrong 
    Doing so allows us to free the pointer, next
  */

  //create copy of string
  char *next = (char*)calloc( ((tk->end)-(tk->start))+1, sizeof(char) );

  strncpy(next,tk->start, ((tk->end)-(tk->start)+1));
  //print string
    printToken( tk, next);

  //resets the string
  tk->start = tk->curr;
  tk->end = tk->curr;

  //creates a copy of the token to return and frees the copy
  char *return_token = NULL;
  return_token = next;
  free(next);

  return return_token;
}

int main(int argc, char **argv) {
  if(argc<2){
    printf("Error: Incorrect Number of Arguments\n");
    return 0;
  }
  TokenizerT *tokenizer = TKCreate(argv[1]);
  
  while(*(tokenizer->curr)!='\0'){
    if(*(tokenizer->curr)=='\0'){
      // break;
    }
    TKGetNextToken(tokenizer);
  }

  TKDestroy(tokenizer);
  return 0;
}

