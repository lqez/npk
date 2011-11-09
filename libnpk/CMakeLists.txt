include_directories( ./external/tea )
include_directories( ./external/xxtea )
include_directories( ./external/zlib )
include_directories( ./include )

set( TEA_SRCS
    ./external/tea/tea.c
    ./external/xxtea/xxtea.c
)

set( ZLIB_UNCOMP_SRCS
    ./external/zlib/adler32.c
    ./external/zlib/crc32.c
    ./external/zlib/infback.c
    ./external/zlib/inffast.c
    ./external/zlib/inflate.c
    ./external/zlib/inftrees.c
    ./external/zlib/uncompr.c
    ./external/zlib/zutil.c
)

set( ZLIB_COMP_SRCS
    ./external/zlib/deflate.c
    ./external/zlib/compress.c
    ./external/zlib/trees.c
)

set( SRCS
    ./src/npk.c
    ./src/npk_common.c
)

set( SRCS_DEV
    ./src/npk_dev.c
)

if( WIN32 )
    if( NOT CMAKE_CL_64 )
        add_definitions( -D_USE_32BIT_TIME_T )
    endif( NOT CMAKE_CL_64 )
    if( BUILD_SHARED_LIBS )
        add_definitions( -DNPK_DLL -DNPK_EXPORTS )
    endif( BUILD_SHARED_LIBS )
endif( WIN32 )

if( USE_ZLIB_PREFIX )
    add_definitions( -DZ_PREFIX )
endif( USE_ZLIB_PREFIX )

if( CASE_SESITIVE )
    add_definitions( -DNPK_CASESENSITIVE )
endif( CASE_SESITIVE )

if( DEV_MODE )
    add_definitions( -DNPK_DEV )
    add_library( libnpk ${SRCS} ${SRCS_DEV} ${TEA_SRCS} ${ZLIB_COMP_SRCS} ${ZLIB_UNCOMP_SRCS} )
else( DEV_MODE )
    add_library( libnpk ${SRCS} ${TEA_SRCS} ${ZLIB_UNCOMP_SRCS} )
endif( DEV_MODE )

enable_testing()
add_subdirectory( tests )
