#include <stdio.h>
#include <stdlib.h>

int sum(int a, int b)
{ 
  unsigned long ebp; 
  asm("\t movl %%ebp,%0" : "=r"(ebp));
  printf("%x \n", ebp);
  int* p = ebp+8; 
  printf("arg %d %x \n",*p,p);
  
  unsigned long* ret = ebp+8;
  unsigned long* ul = *ret;
  printf("ret %x %u \n",*ret, *ul );
  return a+b;
   
} 

int main() 
{ 
  int res = sum(2,3);
}
