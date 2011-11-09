#include "testutil.h"
#include <string>
#include <npk.h>
#include <npk_dev.h>

int libnpk_read_entity_partial( int argc, char * argv [] )
{
    int teakey[4] = {98521,16322,7163,992};

    NPK_PACKAGE pack = npk_package_open( "sample.npk", teakey );
    CHECK( pack != NULL );

    std::string entityNames[2] = { "sample.txt", "tea.txt" };

    for( int i = 0; i < 2; ++i )
    {
        NPK_ENTITY entity = npk_package_get_entity( pack, entityNames[i].c_str() );
        CHECK( entity != NULL );

        NPK_SIZE size = npk_entity_get_size( entity );
        void* buf = malloc( size );

        NPK_FLAG flag;
        npk_entity_get_current_flag( entity, &flag );

        if( flag & NPK_ENTITY_ENCRYPT )
        {
            // offset must be aligned by 8 bytes
            CHECK( (!npk_entity_read_partial( entity, buf, 9, 32 )) );

            // Size can be not aligned by 8 bytes when read end of file
            NPK_SIZE offset = size - (size % 8) - 32;
            CHECK( npk_entity_read_partial( entity, buf, offset, size - offset ) );
            CHECK_EQUAL_STR_WITH_FILE_PARTIAL( (const char*)buf, "sample.txt", offset, size - offset );
        }
        CHECK( npk_entity_read_partial( entity, buf, 8, 32 ) );
        CHECK_EQUAL_STR_WITH_FILE_PARTIAL( (const char*)buf, "sample.txt", 8, 32 );

        free( buf );
    }

    npk_package_close( pack );
    return 0;
}
