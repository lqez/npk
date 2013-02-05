include_directories( ./external/tea )
include_directories( ./external/xxtea )
include_directories( ./external/zlib )
include_directories( ./include )

set( TEA_SRCS
    ./external/tea/tea.c
    ./external/xxtea/xxtea.c
)

set( ZLIB_SRCS
    ./external/zlib/adler32.c
    ./external/zlib/crc32.c
    ./external/zlib/infback.c
    ./external/zlib/inffast.c
    ./external/zlib/inflate.c
    ./external/zlib/inftrees.c
    ./external/zlib/uncompr.c
    ./external/zlib/zutil.c
)

set( ZLIB_SRCS_DEV_MODE
    ./external/zlib/deflate.c
    ./external/zlib/compress.c
    ./external/zlib/trees.c
)

set( NPK_SRCS
    ./src/npk.c
    ./src/npk_common.c
)

set( NPK_SRCS_DEV_MODE
    ./src/npk_dev.c
)

if( WIN32 )
    if( NOT CMAKE_CL_64 )
        add_definitions( -D_USE_32BIT_TIME_T )
    endif( NOT CMAKE_CL_64 )
endif( WIN32 )

if( USE_ZLIB_PREFIX )
    add_definitions( -DZ_PREFIX )
endif( USE_ZLIB_PREFIX )

if( USE_NPK_PREFIX )
    add_definitions( -DNPK_PREFIX )
    add_definitions( -DNPK_PREFIX_NAME=${NPK_PREFIX} )
endif( USE_NPK_PREFIX )

if( CASE_SESITIVE )
    add_definitions( -DNPK_CASESENSITIVE )
endif( CASE_SESITIVE )

if( DEV_MODE )
    add_definitions( -DNPK_DEV )
    set( SRCS ${NPK_SRCS} ${NPK_SRCS_DEV_MODE} ${TEA_SRCS} ${ZLIB_SRCS} ${ZLIB_SRCS_DEV_MODE} )
else( DEV_MODE )
    set( SRCS ${NPK_SRCS} ${TEA_SRCS} ${ZLIB_SRCS} )
endif( DEV_MODE )

add_library( libnpkStatic STATIC ${SRCS} )
add_library( libnpkShared SHARED ${SRCS} )
set_target_properties( libnpkStatic PROPERTIES OUTPUT_NAME libnpk )
set_target_properties( libnpkShared PROPERTIES OUTPUT_NAME libnpk )
set_target_properties( libnpkShared PROPERTIES COMPILE_DEFINITIONS "NPK_DLL")
set_target_properties( libnpkShared PROPERTIES COMPILE_DEFINITIONS "NPK_EXPORTS")

FILE(GLOB headerfiles "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
install(FILES ${headerfiles} DESTINATION include)
install(TARGETS libnpkStatic DESTINATION lib)
install(TARGETS libnpkShared DESTINATION lib)

enable_testing()
add_subdirectory( tests )
