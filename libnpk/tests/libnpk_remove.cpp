#include "testutil.h"
#include <npk.h>
#include <npk_dev.h>
#include <time.h>

int libnpk_remove( int argc, char * argv [] )
{
	const int items = 100;

	srand((unsigned int)time(NULL));
	NPK_PACKAGE pack;
	NPK_ENTITY entity;

	// create a pack
	int teakey[4] = {1,2,3,4};
	CHECK( NPK_SUCCESS == npk_package_alloc( &pack, teakey ) );
	char entityname[items][100];

	for( int t = 0; t < items; ++t )
	{
		sprintf( entityname[t], "%c%c%c%c_%d.txt", rand()%26+65, rand()%26+65, rand()%26+65, rand()%26+65, t );
		CHECK( NPK_SUCCESS == npk_package_add_file( pack, "sample.txt", entityname[t], &entity ) );
	}
	CHECK( NPK_SUCCESS == npk_package_save( pack, "foo.npk", true ) );
	npk_package_close( pack );

	// validation
	CHECK( pack = npk_package_open( "foo.npk", teakey ) );
	for( int t = items-1; t >= 0; --t )
	{
		entity = npk_package_get_entity( pack, entityname[t] );
		CHECK( entity != NULL );
	}

    CHECK( NPK_SUCCESS == npk_package_remove_all_entity( pack ) );

	npk_package_close( pack );
	return 0;
}
