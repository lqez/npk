#include "testutil.h"
#include <npk.h>

int npk_03_flag( int argc, char * argv [] )
{
    system( "../npk test.npk -create --f --k 1:2:3:4" );
    system( "../npk test.npk -add sample.txt sample.txt@zip.txt  sample.txt@tea.txt sample.txt@zipntea.txt --k 1:2:3:4" );
    system( "../npk test.npk -flag zip.txt@C tea.txt@E zipntea.txt@C@E --k 1:2:3:4" );

    int teakey[4] = {1,2,3,4};
    NPK_PACKAGE pack = npk_package_open( "test.npk", teakey );

    CHECK( pack != NULL );

    std::string entityNames[4] = { "sample.txt", "zip.txt", "tea.txt", "zipntea.txt" };

    for( int i = 0; i < 4; ++i )
    {
        NPK_ENTITY entity = npk_package_get_entity( pack, entityNames[i].c_str() );
        CHECK( entity != NULL );

        NPK_SIZE size = npk_entity_get_size( entity );
        void* buf = malloc( size );

        CHECK( npk_entity_read( entity, buf ) );
        CHECK_EQUAL_STR_WITH_FILE( (const char*)buf, "sample.txt" );

        free( buf );
    }

    npk_package_close( pack );

    return 0;
}
