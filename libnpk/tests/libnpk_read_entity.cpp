#include "testutil.h"
#include <string>
#include <npk.h>

int libnpk_read_entity( int argc, char * argv [] )
{
    int teakey[4] = {98521,16322,7163,992};

    NPK_PACKAGE pack = npk_package_open( "sample.npk", teakey );
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
