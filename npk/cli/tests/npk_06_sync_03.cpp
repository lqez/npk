#include "testutil.h"
#include <npk.h>

int npk_06_sync_03( int argc, char * argv [] )
{
    system( "../npk test.npk -create -add sample.txt@remain.txt sample.txt@update.txt sample.txt@notupdate.tmp sample.txt@notdelete.tmp --f --k 1:2:3:4" );
    system( "mkdir sync_test_03" );
    system( "cp sample.txt sync_test_03/add.txt" );
    system( "cp sample.txt sync_test_03/notadd.tmp" );
    system( "cp sample.txt sync_test_03/remain.txt" );
    system( "cp sample2.txt sync_test_03/update.txt" );
    system( "cp sample2.txt sync_test_03/notupdate.tmp" );

    system( "../npk test.npk -sync sync_test_03 --sa --sd --ig *.tmp --k 1:2:3:4" );

    int teakey[4] = {1,2,3,4};
    NPK_PACKAGE pack = npk_package_open( "test.npk", teakey );

    CHECK( pack != NULL );

    NPK_ENTITY entity;
    NPK_SIZE size;
    void* buf;

    entity = npk_package_get_entity( pack, "remain.txt" );
    CHECK( entity != NULL );

    entity = npk_package_get_entity( pack, "add.txt" );
    CHECK( entity != NULL );

    entity = npk_package_get_entity( pack, "notadd.tmp" );
    CHECK( entity == NULL );

    entity = npk_package_get_entity( pack, "notdelete.tmp" );
    CHECK( entity != NULL );

    entity = npk_package_get_entity( pack, "update.txt" );
    CHECK( entity != NULL );

    size = npk_entity_get_size( entity );
    buf = malloc( size );

    CHECK( npk_entity_read( entity, buf ) );
    CHECK_EQUAL_STR_WITH_FILE( (const char*)buf, "sample2.txt" );

    free( buf );

    entity = npk_package_get_entity( pack, "notupdate.tmp" );
    CHECK( entity != NULL );

    size = npk_entity_get_size( entity );
    buf = malloc( size );

    CHECK( npk_entity_read( entity, buf ) );
    CHECK_EQUAL_STR_WITH_FILE( (const char*)buf, "sample.txt" );

    free( buf );

    npk_package_close( pack );

    return 0;
}
