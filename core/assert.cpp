#include "assert.h"
#include <stdio.h>
#include <stdlib.h>

void __impl_assert( int e, const char* expr, const char* file, int line, const char *function ) {
    if( !e ){
        fprintf( stderr, "Assertion failed!\n\n" );
        fprintf( stderr, "File: %s, Line: %d\n", file, line );
        fprintf( stderr, "Function: %s\n\n", function );
        fprintf( stderr, "Expression: %s\n\n", expr );
        fprintf( stderr, "This application has requested the Runtime "
                "to terminate it in an unusual way.\n\n" );
        exit(-1);
    }
}

void __impl_verify( int e, const char* expr, const char* file, int line, const char *function ) {
    if( !e ){
        fprintf( stderr, "Verification failed!\n\n" );
        fprintf( stderr, "File: %s, Line: %d\n", file, line );
        fprintf( stderr, "Function: %s\n\n", function );
        fprintf( stderr, "Expression: %s\n\n", expr );
        fprintf( stderr, "This application has requested the Runtime "
                "to terminate it in an unusual way.\n\n" );
        exit(-1);
    }
}