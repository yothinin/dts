#include <stdio.h>

int addNumber(int a, int b){ //0x55821e0a34
  return a+b;
}

int subNumber(int a, int b){
  return a-b;
}

int main(){
  /* 1
  int(*funcPtr0)(int, int) = addNumber;
  printf("%p -> 5+3 = %d\n", funcPtr0, funcPtr0(5,3));

  int(*funcPtr1)(int, int) = subNumber;
  printf("%p -> 5-2 = %d\n", funcPtr1, funcPtr1(5,3));
  */

  /* 2 
  int (*funcPtr[])(int, int) = {addNumber, subNumber};
  printf("%p -> 5+3 = %d\n", funcPtr[0], funcPtr[0](5,3));
  printf("%p -> 5-2 = %d\n", funcPtr[1], funcPtr[1](5,2));
  */
  /*3
  int ch;
  int chSign[] = {'+', '-'};
  int (*funcPtr[])(int, int) = {addNumber, subNumber};
  printf("Enter your choice \n[0 = add, 1 = sub, or another to exit]: ");
  scanf(" %d", &ch);
  if (ch < 0 || ch > 1) return 0;
  printf("%p -> 5%c3 = %d\n", funcPtr[ch], chSign[ch], funcPtr[ch](5,3));
  */
  
  int ch, a, b;
  char *chFunc[] = {"add", "sub"};
  int chSign[] = {'+', '-'};
  int (*funcPtr[])(int, int) = {addNumber, subNumber};
  
  do {
    printf("Enter your choice\n[0 = add, 1 = sub or another to exit]: ");
    //printf("return: %d\n", scanf("%d", &ch));
    if (!scanf("%d", &ch)) break;
    if (ch < 0 || ch > 1) break;
    
    printf("Enter number to %s: [n n]: ", chFunc[ch]);
    if (!scanf("%d %d", &a, &b)) break;
    
    printf("\n%p -> %d %c %d = %d\n\n", funcPtr[ch], a, chSign[ch], b, funcPtr[ch](a, b));

  } while (1);
  
  return 0;
}
