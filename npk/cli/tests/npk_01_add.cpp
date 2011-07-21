#include "testutil.h"
#include <npk.h>

int npk_01_add( int argc, char * argv [] )
{
    system( "../npk test.npk -create --f --k 1:2:3:4" );
    system( "../npk test.npk -add sample.txt --k 1:2:3:4" );

    int teakey[4] = {1,2,3,4};
    NPK_PACKAGE pack = npk_package_open( "test.npk", teakey );

    CHECK( pack != NULL );

    NPK_ENTITY entity = npk_package_get_entity( pack, "sample.txt" );
    CHECK( entity != NULL );

    NPK_SIZE size = npk_entity_get_size( entity );
    void* buf = malloc( size );

    CHECK( npk_entity_read( entity, buf ) );
    CHECK_EQUAL_STR_WITH_FILE( (const char*)buf, "sample.txt" );

    free( buf );

    npk_package_close( pack );

    return 0;
}
