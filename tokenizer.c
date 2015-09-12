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
  enum entry init_state;
  void (*transition)(struct TokenizerT_);
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

  new_tokenizer->current_state = word;
  new_tokenizer->init_state=alpha;
  new_tokenizer->start = new_tokenizer->string;
  new_tokenizer->curr = new_tokenizer->string;
  new_tokenizer->end = new_tokenizer->string;
  

  return new_tokenizer;
}


void TKDestroy( TokenizerT * tk ) {
  free(tk->string);
  free(tk);
}


void digit_transition(TokenizerT *tk){
  if( (*(tk->start))=='0' ){
    (tk->curr)++;
    if( (*(tk->curr))!='.' && (*(tk->curr))!='x' && (*(tk->curr))!='X' && !isspace((*(tk->curr))) && !isdigit((*(tk->curr))) ){
      tk->current_state = error;
      (tk->curr)++;
    }
  }
  else{
    (tk->curr)++;
    while( !isspace(*(tk->curr))){
      //if(){}
      (tk->curr)++;
    }
    if( isspace(*(tk->curr)) ){
      (tk->curr)++;
    }
  }

}

/*
void float_transition(TokenizerT *tk){
  if(isdigit(*(tk->start)) && (*(tk->start))=='0' &&((tk->start)==(tk->end))){
    printf("Zero\n");

  }
  if((*(tk->curr))=='.' || (*(tk->curr))=='E' || (*(tk->curr))=='e' ){
    (tk->curr)++;

    while(isdigit(*(tk->curr))){
      (tk->curr)++;
    }
    if( isspace(*(tk->curr)) ){
      tk->current_state = float;
      return 0;
    }
    else if(){

    }

  }
}

void error_transition(TokenizerT *tk){

}
*/


char *TKGetNextToken( TokenizerT * tk ) {
  int i;

  /*START TO BRANCHES (FSMs)*/

  if(isdigit(*(tk->start))){
    
    //tk->transition = &digit_transition;   

  }
  if( isspace((*(tk->start))) ){

  }

  printf("state: %u\n", tk->current_state);
 
  int k = (tk->curr) - (tk->string);
  // printf("k: %d\n", k);
  for(i =k; i<strlen(tk->string); i++){
    if(isspace(*(tk->curr))){
      (tk->curr)++;
      break;
    }
    // printf("%d ... %c\n",i,*(tk->curr));
    (tk->curr)++;
    tk->end = tk->curr;
    // printf("curr char %c\n",*(tk->curr));
  }


  /*
  Have to calloc because reusing same memory so often sometimes gives us dirty memory, 
  i.e. printouts that are wrong 
  Doing so allows us to free the pointer, next
  */
  if( isspace(*(tk->curr)) ){
    return 0;
  }
  char *next = (char*)calloc( ((tk->end)-(tk->start))+1, sizeof(char) );
  //printf("malloc size next: %ld\n", (tk->end)-(tk->start));
  strncpy(next,tk->start, (tk->end)-(tk->start));
  //next[((tk->end)-(tk->start))+1]='\0';
  printf("next token to return: %s\n", next);

  /*switch( (tk->current_state) ){
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
    default:
      printf("Idk ");
    break;
    }*/

  printf("%s\n", next);
  tk->start = tk->curr;
  tk->end = tk->curr;
  char *return_token=NULL;
  return_token=next;
  free(next);
  return return_token;
  
  //return next;
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
