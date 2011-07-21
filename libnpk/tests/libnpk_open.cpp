#include "testutil.h"
#include <npk.h>

int libnpk_open( int argc, char * argv [] )
{
    int teakey[4] = {98521,16322,7163,992};

    NPK_PACKAGE pack = npk_package_open( "sample.npk", teakey );
    printf( "%d\n", g_npkError );

    CHECK( pack != 0 );

    npk_package_close( pack );

    return 0;
}
