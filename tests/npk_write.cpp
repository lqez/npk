#include "testutil.h"
#include <npk.h>
#include <npk_dev.h>

int npk_write( int argc, char * argv [] )
{
	long teakey[4] = {1,2,3,4};

	NPK_RESULT res;
	NPK_PACKAGE pack;
	NPK_ENTITY entity;

	// create a pack
	CHECK( NPK_SUCCESS == npk_package_new( &pack, teakey ) );
	CHECK( NPK_SUCCESS == npk_package_add_file( pack, "sample.txt", "sample.txt", &entity ) );
	CHECK( NPK_SUCCESS == npk_package_add_file( pack, "sample.txt", "zip.txt", &entity ) );
	CHECK( NPK_SUCCESS == npk_entity_set_flag( entity, NPK_ENTITY_COMPRESS ) );
	CHECK( NPK_SUCCESS == npk_package_add_file( pack, "sample.txt", "tea.txt", &entity ) );
	CHECK( NPK_SUCCESS == npk_entity_set_flag( entity, NPK_ENTITY_ENCRYPT ) );
	CHECK( NPK_SUCCESS == npk_package_add_file( pack, "sample.txt", "zipntea.txt", &entity ) );
	CHECK( NPK_SUCCESS == npk_entity_set_flag( entity, NPK_ENTITY_COMPRESS | NPK_ENTITY_ENCRYPT | NPK_ENTITY_REVERSE ) );
	CHECK( NPK_SUCCESS == npk_package_save( pack, "foo.npk", true ) );

	npk_package_close( pack );

	// validation
	pack = npk_package_open( "foo.npk", teakey );

	std::string entityNames[4] = { "sample.txt", "zip.txt", "tea.txt", "zipntea.txt" };

	for( int i = 0; i < 4; ++i )
	{
		NPK_ENTITY entity = npk_package_get_entity( pack, entityNames[i].c_str() );
		CHECK( entity != NULL );

		size_t size = npk_entity_get_size( entity );
		void* buf = malloc( size );

		CHECK( npk_entity_read( entity, buf ) );
		CHECK_EQUAL_STR_WITH_FILE( (const char*)buf, "sample.txt" );

		free( buf );
	}


	return 0;
}
