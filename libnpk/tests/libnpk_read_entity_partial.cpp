#include "testutil.h"
#include <string>
#include <npk.h>
#include <npk_dev.h>

int libnpk_read_entity_partial( int argc, char * argv [] )
{
    int teakey[4] = {98521,16322,7163,992};

    NPK_PACKAGE pack = npk_package_open( "sample.npk", teakey );
    NPK_SIZE offset = 0;
    CHECK( pack != NULL );

    std::string entityNames[4] = { "sample.txt", "zip.txt", "tea.txt", "zipntea.txt" };

    for( int i = 0; i < 2; ++i )
    {
        NPK_ENTITY entity = npk_package_get_entity( pack, entityNames[i].c_str() );
        CHECK( entity != NULL );

        NPK_SIZE size = npk_entity_get_size( entity );
        void* buf = malloc( size );

        NPK_FLAG flag;
        npk_entity_get_current_flag( entity, &flag );

        if( flag & NPK_ENTITY_COMPRESS_ZLIB ) {
            // Compressed entity cannot be read partially.
            CHECK( !npk_entity_read_partial( entity, buf, 0, 64 ) );
            continue;
        }

        // read aligned offset with aligned size.
        memset(buf, 0, size);
        CHECK( npk_entity_read_partial( entity, buf, 0, 64 ) );
        CHECK_EQUAL_STR_WITH_FILE_PARTIAL( (const char*)buf, "sample.txt", 0, 64 );

        // npk now supports not-aligned offset partial reading
        memset(buf, 0, size);
        CHECK( npk_entity_read_partial( entity, buf, 5, 32 ) );
        CHECK_EQUAL_STR_WITH_FILE_PARTIAL( (const char*)buf, "sample.txt", 5, 32 );

        // and also not-aligned size partial reading
        memset(buf, 0, size);
        CHECK( npk_entity_read_partial( entity, buf, 8, 23 ) );
        CHECK_EQUAL_STR_WITH_FILE_PARTIAL( (const char*)buf, "sample.txt", 8, 23 );

        // try them together
        memset(buf, 0, size);
        CHECK( npk_entity_read_partial( entity, buf, 11, 31 ) );
        CHECK_EQUAL_STR_WITH_FILE_PARTIAL( (const char*)buf, "sample.txt", 11, 31 );

        // read last part of entity, with aligned offset
        offset = size - (size % 8) - 32;
        CHECK( npk_entity_read_partial( entity, buf, offset, size - offset ) );
        CHECK_EQUAL_STR_WITH_FILE_PARTIAL( (const char*)buf, "sample.txt", offset, size - offset );

        // and again with not-aligend offset
        offset = size - 69;
        CHECK( npk_entity_read_partial( entity, buf, offset, size - offset ) );
        CHECK_EQUAL_STR_WITH_FILE_PARTIAL( (const char*)buf, "sample.txt", offset, size - offset );

        free( buf );
    }

    npk_package_close( pack );
    return 0;
}
