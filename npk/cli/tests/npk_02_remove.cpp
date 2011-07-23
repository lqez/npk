#include "testutil.h"
#include <npk.h>

int npk_02_remove( int argc, char * argv [] )
{
    CMD( "../npk test.npk -create --f --k 1:2:3:4" );
    CMD( "../npk test.npk -add sample.txt --k 1:2:3:4" );
    CMD( "../npk test.npk -remove sample.txt --k 1:2:3:4" );

    int teakey[4] = {1,2,3,4};
    NPK_PACKAGE pack = npk_package_open( "test.npk", teakey );

    CHECK( pack != NULL );

    NPK_ENTITY entity = npk_package_get_entity( pack, "sample.txt" );
    CHECK( entity == NULL );

    npk_package_close( pack );

    return 0;
}
