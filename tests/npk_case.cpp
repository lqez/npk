#include "testutil.h"
#include <npk.h>
#include <npk_dev.h>

/* test for case-sensitive/insensitive entity name */
int npk_case( int argc, char * argv [] )
{
	int teakey[4] = {1,2,3,4};

	NPK_PACKAGE pack;
	NPK_ENTITY entity;

	// validation
	pack = npk_package_open( "case.npk", teakey );
#if defined( NPK_CASESENSITIVE )
	entity = npk_package_get_entity( pack, "AbCdEfGh.txt" );
	CHECK( entity != NULL );
	entity = npk_package_get_entity( pack, "abcdefgh.txt" );
	CHECK( entity == NULL );
#else
	entity = npk_package_get_entity( pack, "AbCdEfGh.txt" );
	CHECK( entity != NULL );
	entity = npk_package_get_entity( pack, "abcdefgh.txt" );
	CHECK( entity != NULL );
#endif
	npk_package_close( pack );

	return 0;
}
