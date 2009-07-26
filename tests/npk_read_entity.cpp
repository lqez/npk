#include "testutil.h"
#include <npk.h>

int npk_read_entity( int argc, char * argv [] )
{
	long teakey[4] = {0,0,0,0};
	NPK_PACKAGE pack = npk_package_open( "foo.npk", teakey );
	NPK_ENTITY entity = npk_package_get_entity( pack, "test.txt" );
	size_t size = npk_entity_get_size( entity );
	void* buf = malloc( size );
	CHECK( npk_entity_read( entity, buf ) );
	CHECK_EQUAL_STR_WITH_FILE( (const char*)buf, "test.txt" );
	free( buf );
	return 0;
}
