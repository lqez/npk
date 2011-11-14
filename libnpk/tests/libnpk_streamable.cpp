#include "testutil.h"
#include <sys/stat.h>
#include <npk.h>
#include <npk_dev.h>

int libnpk_streamable( int argc, char * argv [] )
{
    int teakey[4] = {1,2,3,4};

    NPK_PACKAGE pack;
    NPK_ENTITY entity;

    // create a pack
    CHECK( NPK_SUCCESS == npk_package_alloc( &pack, teakey ) );
    CHECK( NPK_SUCCESS == npk_package_add_file( pack, "sample.txt", "sample.txt", &entity ) );
    CHECK( NPK_SUCCESS == npk_package_add_file( pack, "sample.txt", "tea.txt", &entity ) );
    CHECK( NPK_SUCCESS == npk_entity_set_flag( entity, NPK_ENTITY_ENCRYPT_TEA ) );
    CHECK( NPK_SUCCESS == npk_package_add_file( pack, "sample.txt", "xxtea.txt", &entity ) );
    CHECK( NPK_SUCCESS == npk_entity_set_flag( entity, NPK_ENTITY_ENCRYPT_XXTEA ) );
    CHECK( NPK_SUCCESS == npk_package_add_file( pack, "sample.txt", "zip.txt", &entity ) );
    CHECK( NPK_SUCCESS == npk_entity_set_flag( entity, NPK_ENTITY_COMPRESS_ZLIB ) );
    CHECK( NPK_SUCCESS == npk_package_save( pack, "foo.npk", true ) );

    npk_package_close( pack );

    // simulate download
    int rh = open( "foo.npk", O_RDONLY );
    size_t filesize = npk_seek( rh, 0, SEEK_END );
    npk_seek( rh, 0, SEEK_SET );

    int wh = open( "foo_2.npk", O_CREAT | O_RDWR | O_TRUNC, S_IREAD | S_IWRITE );

    // validation
    std::string entityNames[4] = { "sample.txt", "tea.txt", "xxtea.txt", "zip.txt" };
    pack = 0;
    int i = 0;
    size_t offset = 0;
    char buf[255];

    while( offset < filesize )
    {
        size_t r = rand()%64;
        if( r + offset > filesize )
            r = filesize - offset;

        read( rh, &buf, sizeof(char)*r );
        write( wh, buf, sizeof(char)*r );
        printf( "offset %ld, reading %ld byte(s).\n", offset, r );
        offset += r;

        if( pack == 0 )
        {
            pack = npk_package_open( "foo_2.npk", teakey );
            if( pack != 0 )
                printf( "   package loaded.\n" );
        }
        else
        {
            NPK_ENTITY entity = npk_package_get_entity( pack, entityNames[i].c_str() );
            CHECK( entity != NULL );

            NPK_SIZE size = npk_entity_get_size( entity );
            if( npk_entity_is_ready( entity ) )
            {
                printf( "   entity %s ready.\n", entityNames[i].c_str() );
                void* buf = malloc( size );

                CHECK( npk_entity_read( entity, buf ) );
                CHECK_EQUAL_STR_WITH_FILE( (const char*)buf, "sample.txt" );

                free( buf );
                ++i;
            }
        }
    }

    npk_package_close( pack );
    close( wh );
    close( rh );

    return 0;
}
