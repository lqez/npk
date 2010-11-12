#include "testutil.h"
#include <string>
#include <npk.h>
#include <fcntl.h>

int npk_using_fd( int argc, char * argv [] )
{
	int teakey[4] = {98521,16322,7163,992};

	int fd = open( "sample_with_padding.bin", O_BINARY | O_RDONLY );
	printf( "fd:%d\n", fd );

	/* file descriptor, offset, size, key */
	NPK_PACKAGE pack = npk_package_open_with_fd( "sample.npk", fd, 1000, 6771, teakey ); 
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

	close( fd );
	return 0;
}
