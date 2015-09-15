/*
 * tokenizer.c
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

enum state{
  word,
  decimal,
  octal,
  hexadecimal,
  floating_point,
  C_operator,
  error
};
enum entry{
  alpha,
  zero,
  oneToNine,
  symbol
};

struct TokenizerT_ {
  char *string;
  enum state current_state;
  int accept_state;

  void (*transition)();
  char *start;
  char *curr;
  char *end;
};

typedef struct TokenizerT_ TokenizerT;


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

//C-OPs
/*if( ( *(tk->start)>=33 && *(tk->start)<=47 )||( *(tk->start)>=58 && *(tk->start)<=64 )||( *(tk->start)>=91 && *(tk->start)<=97 )||( *(tk->start)>=123 && *(tk->start)<=126 ) ){
           
  if( (*((tk->start)+1) >=33 && *((tk->start)+1)<=47)||(*((tk->start)+1) >=58 && *((tk->start)+1)<=64)||(*((tk->start)+1) >=91 && *((tk->start)+1)<=97)||(*((tk->start)+1) >=123 && *((tk->start)+1)<=126) ) {
   //Doesn't handle malformed c_op tokens that are more than 2 characters  
  tk->current_state=C_operator;
  }
  else{
   printf("error\n");
   tk->current_state=error;
     
  }
            
  }*/


void isWord(TokenizerT * tk ){

}
void isDecimal(TokenizerT * tk ){
  // i need to check if i transition to float due to next char

}
void isOctal(TokenizerT * tk ){

}
void isHexadecimal(TokenizerT * tk ){

}
void isFloat(TokenizerT * tk ){

}
void isCToken(TokenizerT * tk ){

}
void isMal(TokenizerT * tk ){

}
void DestroySpace (TokenizerT * tk){    
  if(tk->accept_state==1){
    (tk->curr)++;
    break;
  }
  (tk->curr)++;
  (tk->end) = (tk->curr);
  (tk->start) = (tk->curr);
  //(tk->curr)++;

}

//SET STATE OF TOKEN AT THE END

char *TKGetNextToken( TokenizerT * tk ) {
  /* debug
  int ascii = (*(tk->curr));
  printf("%c %d\n", (*(tk->curr)), ascii);
  */

  //skips past blank space
  if(isspace(*(tk->start) ){
      tk->start++;
      tk->curr = tk->start;
      tk->end = tk->start;
      break;
    }
    

    // standard case functions here
    if(isdigit(*(tk->start))){
      if(*(tk->start) == '0'){
        switch( *((tk->start) + 1) ){
	case '.':
	  tk->curr++;
	  tk->end = tk-> curr;
	  isFloat(tk);
	  break;
	case 'x':
	  tk->curr++;
	  tk->end = tk-> curr;
	  isHexadecimal(tk);
	  break;
          case 'X'
            tk->curr++;
	  tk->end = tk-> curr;
	  isHexadecimal(tk);
	  break;
	case ' ':
	  //this num is a zero. What do we do?
	  break;
	default :
	  tk->curr++;
	  tk->end = tk-> curr;
	  isOctal(tk);
	  break;            
        }
      }
      else{
        tk->curr++;
        tk->end = tk-> curr;
        isDecimal(tk);
      }
    }

    if(ispunct(*(tk->start))){
      //COperator

      //Quotes

      //Comments
    }

    if(isalpha){
      // keywords functions here

      //Words
    }
    /*
    Have to calloc because reusing same memory so often sometimes gives us dirty memory, 
    i.e. printouts that are wrong 
    Doing so allows us to free the pointer, next
    */

    //create copy of string
    char *next = (char*)calloc( ((tk->end)-(tk->start))+1, sizeof(char) );
    strncpy(next,tk->start, (tk->end)-(tk->start));

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


  void printToken(TokenizerT *tk, char *next){
    switch( (tk->current_state) ){
    case 0:
      printf("word ");
      break;
    case 1:
      printf("decimal ");
      break;
    case 2:
      printf("octal ");
      break;
    case 3:
      printf("hexadecimal ");
      break;
    case 4:
      printf("floating point ");
      break;
    case 5:
      printf("C_operator ");
      //Find the symbol pattern and print
      break;
    case 6:
      printf("error ");
      break;    
    default:
      printf("Idk? ");
      break;
    }

    printf("%s length: %lu\n", next, strlen(next));
  }


  int main(int argc, char **argv) {

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
