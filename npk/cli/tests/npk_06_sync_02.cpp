#include "testutil.h"
#include <npk.h>

int npk_06_sync_02( int argc, char * argv [] )
{
    CMD( "../npk test.npk -create -add sample.txt@remain.txt sample.txt@update.txt sample.txt@delete.txt --f --k 1:2:3:4" );
    CMD( "mkdir sync_test_02" );
    CP( "sample.txt", "sync_test_02/add.txt" );
    CP( "sample.txt", "sync_test_02/remain.txt" );
    CP( "sample2.txt", "sync_test_02/update.txt" );

    CMD( "../npk test.npk -sync sync_test_02 --sa --sd --k 1:2:3:4" );

    int teakey[4] = {1,2,3,4};
    NPK_PACKAGE pack = npk_package_open( "test.npk", teakey );

    CHECK( pack != NULL );

    NPK_ENTITY entity;

    entity = npk_package_get_entity( pack, "remain.txt" );
    CHECK( entity != NULL );

    entity = npk_package_get_entity( pack, "add.txt" );
    CHECK( entity != NULL );

    entity = npk_package_get_entity( pack, "delete.txt" );
    CHECK( entity == NULL );

    entity = npk_package_get_entity( pack, "update.txt" );
    CHECK( entity != NULL );

    NPK_SIZE size = npk_entity_get_size( entity );
    void* buf = malloc( size );

    CHECK( npk_entity_read( entity, buf ) );
    CHECK_EQUAL_STR_WITH_FILE( (const char*)buf, "sample2.txt" );

    free( buf );

    npk_package_close( pack );

    return 0;
}
