#include "testutil.h"
#include <npk.h>
#include <npk_dev.h>
#include <time.h>

int libnpk_hash( int argc, char * argv [] )
{
	const int items = 10000;

	srand((unsigned int)time(NULL));
	NPK_PACKAGE pack;
	NPK_ENTITY entity;

	// create a pack
	int teakey[4] = {1,2,3,4};
	CHECK( NPK_SUCCESS == npk_package_alloc( &pack, teakey ) );
	char entityname[items][100];
    char ventityname[100];

	for( int t = 0; t < items; ++t )
	{
		sprintf( entityname[t], "a\\b/%c%c%c%c_%d.TXT", rand()%26+65, rand()%26+65, rand()%26+65, rand()%26+65, t );
		CHECK( NPK_SUCCESS == npk_package_add_file( pack, "sample.txt", entityname[t], &entity ) );
	}
	CHECK( NPK_SUCCESS == npk_package_save( pack, "foo_hash.npk", true ) );
	npk_package_close( pack );

	// validation
	CHECK( pack = npk_package_open( "foo_hash.npk", teakey ) );
	for( int t = items-1; t >= 0; --t )
	{
        strcpy( ventityname, entityname[t] );
        char*v = ventityname;
        while( *v )
        {
            if( *v >= 'A' && *v <= 'Z' )
                *v = *v + 32;
            ++v;
        }

#ifdef NPK_CASESENSITIVE
		entity = npk_package_get_entity( pack, ventityname );
		CHECK( entity == NULL );
#else
		entity = npk_package_get_entity( pack, ventityname );
		CHECK( entity != NULL );
#endif
		entity = npk_package_get_entity( pack, entityname[t] );
		CHECK( entity != NULL );
	}

	// displaying hash bucket status
	int b_min = items;
	int b_max = 0;
	int b_sum = 0;

	NPK_PACKAGEBODY* pb = (NPK_PACKAGEBODY*)pack;
	for( int i = 0; i < NPK_HASH_BUCKETS; ++i )
	{
		int c = 0;
		NPK_ENTITYBODY* ep = pb->bucket_[i]->pEntityHead_;
		while( ep )
		{
			++c;
			ep = ep->nextInBucket_;
		}
		if( c > b_max ) b_max = c;
		if( c < b_min ) b_min = c;
		b_sum += c;
	}
	printf( "buckets:%d\n", NPK_HASH_BUCKETS );
	printf( "total:%d\n", b_sum );
	printf( "min:%d\n", b_min );
	printf( "max:%d\n", b_max );
	printf( "avg:%.2f\n", (double)b_sum/NPK_HASH_BUCKETS );

	npk_package_close( pack );
	return 0;
}
