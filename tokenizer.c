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


struct TokenizerT_ {
  char *string;
  enum state current_state;
  int length;
};

typedef struct TokenizerT_ TokenizerT;

/*
 * TKCreate creates a new TokenizerT object for a given token stream
 * (given as a string).
 * 
 * TKCreate should copy the arguments so that it is not dependent on
 * them staying immutable after returning.  (In the future, this may change
 * to increase efficiency.)
 *
 * If the function succeeds, it returns a non-NULL TokenizerT.
 * Else it returns NULL.
 *
 * You need to fill in this function as part of your implementation.
 */
TokenizerT *TKCreate( char * ts ) {

  if(ts==NULL){
    return NULL;
  }

  TokenizerT *new_tokenizer =(TokenizerT*)malloc(sizeof(TokenizerT));

  new_tokenizer->string = (char*)malloc(strlen(ts)*sizeof(char));
  strcpy(new_tokenizer->string,ts);

  new_tokenizer->current_state = word;
  new_tokenizer->length = 0;
  

  return new_tokenizer;
}




/*
 * TKDestroy destroys a TokenizerT object.  It should free all dynamically
 * allocated memory that is part of the object being destroyed.
 *
 * You need to fill in this function as part of your implementation.
 */
void TKDestroy( TokenizerT * tk ) {
  free(tk->string);
  free(tk);
}




/*
 * TKGetNextToken returns the next token from the token stream as a
 * character string.  Space for the returned token should be dynamically
 * allocated.  The caller is responsible for freeing the space once it is
 * no longer needed.
 *
 * If the function succeeds, it returns a C string (delimited by '\0')
 * containing the token.  Else it returns 0.
 *
 * You need to fill in this function as part of your implementation.
 */
char *TKGetNextToken( TokenizerT * tk ) {
  int i;
  int start = strlen(tk->string) - tk->length;
  //printf("start: %d\n", start);
  char *next = (char*)malloc(start*sizeof(char));

  for(i =start; i<strlen(tk->string); i++){

    next[i] = tk->string[i];
    //printf("%d %c\n", i, next[i]);

    if(isalpha(tk->string[i])){
      tk->current_state=word;
      //printf("%u\n", tk->current_state);
      
      //tk->current_state = word;
    }
    else if(isdigit(tk->string[i])){
      /*
	tk->current_state=decimal;
	printf("%u\n", tk->current_state);
      */

    }
    else if(ispunct(tk->string[i])){
      tk->current_state=C_operator;
      /*
	printf("%u\n", tk->current_state);
      */

    }
    else if(isspace(tk->string[i])){
      /*
	printf("whitespace\n");
      */
    }
    else{
      /*
	tk->current_state=error;
	printf("%u\n", tk->current_state);
      */
    }
  }
  
  free(next);

  /*
Right now the function returns "hi" instead of the actual tokens because we don't have real tokens yet
  */
  return "hi";
}


/*
Table-driven vs State Design Pattern?
*/

void word_transition(void){

}
void digit_transition(void){
  
}
void punct_transition(void){
  
}
void whitespace_transition(void){
  
}

/*void function_a(void){
  printf("pointed to function_a!\n");
  }*/


/*
 * main will have a string argument (in argv[1]).
 * The string argument contains the tokens.
 * Print out the tokens in the second string in left-to-right order.
 * Each token should be printed on a separate line.
 */
int main(int argc, char **argv) {

  /*void (*functionPtr)();
    functionPtr = &function_a;
    (*functionPtr)();
  */

  TokenizerT *tokenizer = TKCreate(argv[1]);


  //printf("tokenizer string: %s | strleng: %lu | state: %u\n",tokenizer->string,strlen(tokenizer->string),tokenizer->current_state);
  
  tokenizer->length = strlen(argv[1]);
  
  while(tokenizer->length>0){
    char *next_token = TKGetNextToken(tokenizer);
    tokenizer->length = tokenizer->length - strlen(next_token);
  }

  TKDestroy(tokenizer);


  return 0;
}




