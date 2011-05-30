#include "testutil.h"
#include <npk.h>
#include <npk_dev.h>

int npk_export( int argc, char * argv [] )
{
	int teakey[4] = {98521,16322,7163,992};

	NPK_PACKAGE pack = npk_package_open( "sample.npk", teakey );
	CHECK( pack != NULL );

	std::string entityNames[4] = { "sample.txt", "zip.txt", "tea.txt", "zipntea.txt" };

	for( int i = 0; i < 4; ++i )
	{
		NPK_ENTITY entity = npk_package_get_entity( pack, entityNames[i].c_str() );
		CHECK( entity != NULL );

		std::string exported = "exported." + entityNames[i];
		CHECK( NPK_SUCCESS == npk_entity_export( entity, exported.c_str(), true ) );

		NPK_SIZE size = npk_entity_get_size( entity );
		void* buf = malloc( size );

		CHECK( npk_entity_read( entity, buf ) );
		CHECK_EQUAL_STR_WITH_FILE( (const char*)buf, exported.c_str() );

		free( buf );
	}

	npk_package_close( pack );
	return 0;
}
