#include "assert.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstdarg>

/* __assertion_failed */
static void __assertion_failed( 
    const char *fail, const char *expr, const char *file,
    int line, const char *function, const char *msg, va_list argptr)
{
    fprintf( stderr, "%s\n\n", fail );
    fprintf( stderr, "File: %s, Line: %d\n", file, line );
    fprintf( stderr, "Function: %s\n\n", function );
    fprintf( stderr, "Expression: %s\n\n", expr );
    if ( msg ) {
        fprintf( stderr, "Message: " );
        vfprintf( stderr, msg, argptr );
        fprintf( stderr, "\n\n" );
    }
    fprintf( stderr, "This application has requested the Runtime "
            "to terminate it in an unusual way.\n\n" );
    exit(-1);
}

void __impl_asserta( int e, const char *expr, const char *file, int line, const char *function, const char *fmt, ... )
{
    if( !e ) {
        va_list argptr;
        va_start( argptr, fmt );
        __assertion_failed( "Assertion failed!", expr, file, line, function, fmt, argptr );
        va_end( argptr );
    }
}

/* __impl_assert */
void __impl_assert( int e, const char* expr, const char* file, int line, const char *function )
{
    if( !e ) {
        __assertion_failed( "Assertion failed!", expr, file, line, function, nullptr, nullptr );
    }
}

/* __impl_verify */
void __impl_verify( int e, const char* expr, const char* file, int line, const char *function )
{
    if( !e ){
        __assertion_failed( "Verification failed!", expr, file, line, function, nullptr, nullptr );
    }
}
