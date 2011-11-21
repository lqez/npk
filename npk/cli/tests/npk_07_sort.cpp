#include "testutil.h"
#include <npk.h>
#include <npk_dev.h>

int npk_07_sort( int argc, char * argv [] )
{
    CMD( "../npk test.npk -create -add sample.txt@02.jpg sample.txt@01_thumb.jpg sample.txt@02_thumb.jpg sample.txt@01.jpg sample.txt@test.xml --f --k 1:2:3:4" );
    CMD( "../npk test.npk -sort *.xml *_thumb.*@DESC --v --k 1:2:3:4" );

    int teakey[4] = {1,2,3,4};
    NPK_PACKAGE pack = npk_package_open( "test.npk", teakey );

    CHECK( pack != NULL );

    NPK_ENTITYBODY* eb = (NPK_ENTITYBODY*)npk_package_get_first_entity( pack );
    std::string entityNames[5] = { "test.xml", "02_thumb.jpg", "01_thumb.jpg", "01.jpg", "02.jpg" };

    int i = 0;
    while( eb )
    {
        CHECK_EQUAL_STR( entityNames[i].c_str(), eb->name_ );
        eb = eb->next_;
        ++i;
    }
    npk_package_close( pack );

    return 0;
}
