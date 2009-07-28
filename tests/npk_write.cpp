#include "testutil.h"
#include <npk.h>
#include <npk_dev.h>

int npk_write( int argc, char * argv [] )
{
	long teakey[4] = {0,0,0,0};
	NPK_RESULT res;
	NPK_PACKAGE pack;
	NPK_ENTITY entity;

	// create a pack
	res = npk_package_new( &pack, teakey );
	CHECK( res == NPK_SUCCESS );
	res = npk_package_add_file( pack, "test.txt", "test.txt", &entity );
	CHECK( res == NPK_SUCCESS );
	res = npk_package_save( pack, "foo2.npk", true );
	CHECK( res == NPK_SUCCESS );
	npk_package_close( pack );

	// validation
	pack = npk_package_open( "foo2.npk", teakey );
	entity = npk_package_get_entity( pack, "test.txt" );
	size_t size = npk_entity_get_size( entity );
	void* buf = malloc( size );
	CHECK( npk_entity_read( entity, buf ) );
	CHECK_EQUAL_STR_WITH_FILE( (const char*)buf, "test.txt" );
	free( buf );
	npk_package_close( pack );

	return 0;
}
