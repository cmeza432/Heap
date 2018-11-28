#include <string.h>
#include <stdlib.h>
#include <time.h>

int main()
{
  int * int_array[1024];
  int * rand_array[1024];
  int * pool;

  int i = 0;

  srand( time( 0 ) );

  pool = (int*)malloc( 4096 );
  free( pool );

  for( i = 0; i < 1024; i++ )
  {
    int_array[i] = (int*)malloc( sizeof( int ) );
  }

  for( i = 0; i < 1024; i++ )
  { 
    if( i % 2 == 0 )
    {
       free( int_array[i] );
    }
  }

  for( i = 0; i < 1024; i++ )
  { 
    rand_array[i] = (int*) malloc ( rand()%8192 ) ;
  }

  

  return 0;
}