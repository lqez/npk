/* testutil.h from haje01's GX library */
#ifndef __TESTUTIL_H__
#define __TESTUTIL_H__

#include <iostream>
#include <fcntl.h>
#include <stdio.h>
#if defined( WIN32 )
#include <io.h>
#include <sys/utime.h>
#pragma warning ( disable : 4819 )
#pragma warning ( disable : 4996 )
#else
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#endif

#ifndef O_BINARY
#define O_BINARY 0
#endif

#if defined( WIN32 )
#define CHECK(x) \
    {if(!(x)) { printf("Assertion Failed : %s, %d\n",__FILE__,__LINE__); \
    __debugbreak(); }}
#define CMD(x) \
    { \
        char buf[512]; \
        strcpy( buf, x ); \
        size_t l = strlen(buf); \
        for( size_t i = 0; i < l; ++i ) \
            if( buf[i] == '/' ) buf[i] = '\\'; \
        system(buf); \
    }
#define CP(x,y) \
    { \
        char buf[512]; \
        sprintf( buf, "copy %s %s %%Y", x, y ); \
        size_t l = strlen(buf); \
        for( size_t i = 0; i < l; ++i ) { \
            if( buf[i] == '/' ) buf[i] = '\\'; \
            if( buf[i] == '%' ) buf[i] = '/'; \
		} \
        system(buf); \
		_utime(y, NULL); \
    }
#else
#define CHECK(x) \
    {if(!(x)) { printf("Assertion Failed : %s, %d\n",__FILE__,__LINE__); \
        __asm__("int $0x03");}}
#define CMD(x) \
    system(x);
#define CP(x,y) \
    { \
        char buf[512]; \
        sprintf( buf, "cp %s %s", x, y ); \
        CMD(buf); \
    }
#endif

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#define Sleep(x) usleep((x)*1000)
#endif

#define CHECK_CLOSE( M_A, M_B, M_EPSILON ) \
    CHECK( fabs( M_A - M_B ) < M_EPSILON );
#define CHECK_EQUAL( M_A, M_B ) \
    CHECK( M_A == M_B )
#define CHECK_EQUAL_STR( M_A, M_B ) \
    CHECK( strcmp((M_A), (M_B)) == 0 )
#define CHECK_EQUAL_STR_SIZE( M_A, M_B, S ) \
    CHECK( strncmp((M_A), (M_B), (S)) == 0 )

static void CHECK_EQUAL_STR_WITH_FILE( const char* src, const char* filename )
{
    int     h;
    char*   buf;
    size_t  size;
    
    h = open( filename, O_RDONLY | O_BINARY );
    CHECK( h >= 0 );

    size = lseek( h, 0, SEEK_END );
    CHECK( size != 0 );

    lseek( h, 0, SEEK_SET );

    buf = (char*)malloc( size+1 );
    CHECK( buf != NULL );

    // put '\0' at last
    ((char*)buf)[size] = '\0';

    CHECK_EQUAL( size, read( h, buf, size ) );

    CHECK_EQUAL_STR_SIZE( buf, src, size );

    free( buf );
    close( h );
}

#endif // __TESTUTIL_H__
