#include "testutil.h"
#include <npk.h>

int npk_open( int argc, char * argv [] )
{
	long teakey[4] = {98521,16322,7163,992};

	char cwd_path[1024];
	getcwd(cwd_path, 1024);
	printf( "cwd:%s\n", cwd_path );

	NPK_PACKAGE pack = npk_package_open( "sample.npk", teakey );
	CHECK( pack != 0 );

	npk_package_close( pack );

	return 0;
}
