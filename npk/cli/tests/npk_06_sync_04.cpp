#include "testutil.h"
#include <npk.h>
#include <npk_dev.h>

int npk_06_sync_04( int argc, char * argv [] )
{
    CMD( "mkdir sync_test_04" );
    CMD( "mkdir sync_test_04/nested" );
    CP( "sample2.txt", "sync_test_04/add1.txt" );
    CP( "sample2.txt", "sync_test_04/add2.test" );
    CP( "sample2.", "txt sync_test_04/nested/add3.tmp" );

    CMD( "../npk test.npk -create -sync sync_test_04 --sa -flag \"*.txt@C\" \"add2*@E\" \"*add3.tmp@C@E\" --v --f --k 1:2:3:4" );

    int teakey[4] = {1,2,3,4};
    NPK_PACKAGE pack = npk_package_open( "test.npk", teakey );

    CHECK( pack != NULL );

    NPK_ENTITY entity;
    NPK_FLAG flag;

    entity = npk_package_get_entity( pack, "add1.txt" );
    CHECK( entity != NULL );
    npk_entity_get_current_flag( entity, &flag );
    CHECK( flag & ( NPK_ENTITY_COMPRESS_ZLIB ) ) 

    entity = npk_package_get_entity( pack, "add2.test" );
    CHECK( entity != NULL );
    npk_entity_get_current_flag( entity, &flag );
    CHECK( flag & ( NPK_ENTITY_ENCRYPT_TEA ) ) 

    entity = npk_package_get_entity( pack, "nested/add3.tmp" );
    CHECK( entity != NULL );
    npk_entity_get_current_flag( entity, &flag );
    CHECK( flag & ( NPK_ENTITY_ENCRYPT_TEA | NPK_ENTITY_COMPRESS_ZLIB ) ) 

    npk_package_close( pack );

    return 0;
}
