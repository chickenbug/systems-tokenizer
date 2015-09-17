/*
 * tokenizer.c
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "tokenizer.h"

/*
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

  char *start;
  char *curr;
  char *end;
};

typedef struct TokenizerT_ TokenizerT;
*/

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

void isMal(TokenizerT * tk ){
  // Need to deal with incomplete hex and float tokens
  // print error statement
  //printf("%c\n", *(tk->curr));
  while( !isspace( *((tk->curr)+1) ) && *((tk->curr)+1) !='\0'){
    tk->curr++;
  }
  tk->current_state = error;
  tk->curr++;
  tk->start = tk->curr;
  tk->end = tk->curr;
  return;

}

void isWord(TokenizerT * tk ){
  //Punctuation is mal for now, deal with () [] // /**/ etc. later
  if( isspace(*((tk->curr)+1)) || *((tk->curr)+1) =='\0' ){
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
    (tk->curr)++;
    isMal(tk);
  }

}
void isDecimal(TokenizerT * tk ){
  // i need to check if i transition to float due to next char
  //Punctuation is mal for now, deal with () [] // /**/ etc. later
  if( isspace( *((tk->curr)+1) ) || *((tk->curr)+1) =='\0' ){
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
    (tk->curr)++;
    isMal(tk);
  }
  

}
void isOctal(TokenizerT * tk ){
  //Punctuation is mal for now, deal with () [] // /**/ etc. later
  if( isspace( *((tk->curr)+1) ) || *((tk->curr)+1) =='\0' ){
    tk->current_state = octal;
    (tk->end) = (tk->curr);
    tk->curr++;
    return;
  }
  else if( isdigit( *((tk->curr)+1) )){
    switch( *((tk->curr)+1) ){
    case '8':
      isMal(tk);
      break;
    case '9':
      isMal(tk);
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

void isHexadecimal(TokenizerT * tk ){
  if( isspace( *((tk->curr)+1) ) || *((tk->curr)+1) =='\0' ){
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

void isFloat(TokenizerT * tk ){
  if( isspace( *((tk->curr)+1) ) || *((tk->curr)+1) =='\0' ){
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

void isFloatE(TokenizerT * tk ){
  
  if( isspace( *((tk->curr)+1) ) || *((tk->curr)+1) =='\0' ){
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
      //return;
    }
  }
  else{
    printf("here \n");
    (tk->curr)++;
    isMal(tk);
  }    
}

void isCToken(TokenizerT * tk ){
  //use recursive advantage to add +/- to valid float token in CToken function
  //+/- float only applies if white space is in front; otherwise we just take the +/- and discard the rest of token
  //

  //if( *(tk->curr) ==33 ||  *(tk->curr) ==58 || ( *(tk->curr)>=37&&*(tk->curr)<=47 ) || ( *(tk->curr)>=60&&*(tk->curr)<=63 ) || ( *(tk->curr)>=91&&*(tk->curr)<=94 ) || ( *(tk->curr)>=123&&*(tk->curr)<=126 )  ){
      switch( *(tk->curr) ){
      case '-':
	if( ((*(tk->curr)+1))=='>' ){
	  tk->current_state = structuremember;
	  tk->curr+=2;
	  tk->end = tk->curr-1;
	}
	else if( (*((tk->curr)+1)) == '-' ){
	  tk->current_state = dec;
	  tk->curr+=2;
	  tk->end = tk->curr-1;
	}
	else if( (*((tk->curr)+1))== '=' ){
	  tk->current_state = minusequals;
	  tk->curr+=2;
	  tk->end = tk->curr-1;
	}
	else{
	  tk->current_state = minus;
	  tk->curr++;
	}
	break;
      case '+':
	//plus equals
	//++
	tk->current_state = add;
	tk->curr++;
	break;
      case '=':
	//equals equals
	tk->current_state = assignmentoperator;
	tk->curr++;
	break;
      case '*':
	//times equals
	tk->current_state = multiply;
	tk->curr++;
	break;
      case '%':
	//modulo equals
	tk->current_state = modulo;
	tk->curr++;
	break;
      case '>':
	//>=
	//>>=
	//>>
	tk->current_state = greaterthan;
	tk->curr++;
	break;
      case '<':
	//<=
	//<<
	//<<=
	tk->current_state = lessthan;
	tk->curr++;
	break;
      case '/':
	if(*((tk->curr)+1) == '='){
	  tk->current_state = divideequals;
	  tk->curr+=2;
	  tk->end = tk->curr-1;
	  }
	else{
	  tk->current_state=divide;
	  tk->curr++;
	} 
	break;
      case '(':
	break;
      case ')':
	break;
      case '.':
	break;
      case '&':
	//&=
	//&&
	break;
      case '!':
	//!=
	break;
      case '~':
	break;
      case '^':
	//^=
	break;
      case '[':
	break;
      case ']':
	break;
      case '?':
	break;
      case ':':
	break;
      case ',':
	break;
      case '|':
	// ||
	break;
      //deal with sizeof in words

      default:
	(tk->curr)++;
	isCToken(tk);
	break;
      }
  // }

      // }
     /* else{
    (tk->curr)++;
    isMal(tk);
    }*/
  return;
}

//actually use this instead of returning null

void DestroySpace (TokenizerT * tk){    
  if(tk->accept_state==1){
    (tk->curr)++;
    return;
  }
  (tk->curr)++;
  (tk->end) = (tk->curr);
  (tk->start) = (tk->curr);
  //(tk->curr)++;

}



void printToken(TokenizerT *tk, char *next){
  switch( (tk->current_state) ){
  case 0:
    printf("zero");
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
  case 6:
    printf("error");
    //Find the symbol pattern and print
    break;
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
    printf("minus");
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
    printf("cast");
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
    printf("less or equal");
    break;
  case 30:
    printf("greater or equal");
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
    printf("true operator");
    break;
  case 38:
    printf("false operator");
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
  default:
    printf("Unknown State");
    break;
  }

  printf(" %s\n", next);
}



//SET STATE OF TOKEN AT THE END

char *TKGetNextToken( TokenizerT * tk ) {

  //skips past blank space
  //Change to destroy later to be safe
  if( isspace(*(tk->start)) ){
    tk->start++;
    tk->curr = tk->start;
    tk->end = tk->start;
    return NULL;
  }
    

  // standard case functions here
  if(isdigit(*(tk->curr))){
    if(*(tk->curr) == '0'){
      //check if space or end of string (zero)
      if( isspace( *((tk->curr)+1) ) || *((tk->curr)+1) =='\0' ){
	tk->current_state = zero;
	tk->curr++;
      } 
      else{
	switch( *((tk->curr) + 1) ){
	case '.':
	  tk->curr++;
	  isFloat(tk);
	  break;
	case 'x':
	  tk->curr++;
	  isHexadecimal(tk);
	  break;
	case 'X':
	  tk->curr++;
	  isHexadecimal(tk);
	  break;
	case ' ':
	  //this num is a zero. What do we do?
	  break;
	  // default case when you hit a zero is octal
	default :
	  isOctal(tk);
	  break;            
	}
      }
    }
    else{
      isDecimal(tk);
    }
  }

  else if(ispunct(*(tk->start))){
    //COperator
    isCToken(tk);

    //Quotes

    //Comments
  }

  else if(isalpha(*(tk->start))){
    // keywords functions here

    //Words
    isWord(tk);
  }
  else{
    printf("something is very wrong\n");
  }
  /*
    Have to calloc because reusing same memory so often sometimes gives us dirty memory, 
    i.e. printouts that are wrong 
    Doing so allows us to free the pointer, next
  */

  //prevent an error to get memory allocation and get returned as a token
  if(tk->current_state==7){
    //print temporary message
    printf("invalid token\n");
    return NULL;
  }

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
