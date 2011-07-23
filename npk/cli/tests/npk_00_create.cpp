#include "testutil.h"
#include <npk.h>

int npk_00_create( int argc, char * argv [] )
{
    CMD( "../npk test.npk -create --f --k 1:2:3:4" );

    int teakey[4] = {1,2,3,4};
    NPK_PACKAGE pack = npk_package_open( "test.npk", teakey );

    CHECK( pack != 0 );

    npk_package_close( pack );

    return 0;
}
