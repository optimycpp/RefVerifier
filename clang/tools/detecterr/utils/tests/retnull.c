#include<stdio.h>
int* foo(int a) {
  if (a != -2) {
    printf("Hello\n");
    if (a<0) {
      return &a;
    }
  }  
  return NULL;
}
