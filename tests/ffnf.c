#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Test first fit and next fit
int main()
{
  char * ptr1 = ( char * ) malloc ( 1000 );
  char * ptr2 = ( char * ) malloc ( 6000 );
  char * ptr3 = ( char * ) malloc ( 1000 );

  printf("First fit should pick this one: %p\n", ptr1 );
  printf("Next fit should pick this one: %p\n", ptr3 );

  free( ptr1 );
  free( ptr2 );
  free( ptr3 );

  char * ptr5 = ( char * ) malloc ( 6000 );
  char * ptr4 = ( char * ) malloc ( 1000 );
  printf("Chosen address: %p\n", ptr4 );

  return 0;
}
