cmake_minimum_required ( VERSION 2.6 )

project ( npk )

set( CMAKE_STATIC_LIBRARY_PREFIX "" )
set( CMAKE_SHARED_LIBRARY_PREFIX "" )

set_directory_properties(
    PROPERTY DEBUG_POSTFIX "d"
)

set( USE_ZLIB_PREFIX OFF
    CACHE BOOL "Turn on to use zlib's Z_PREFIX option (to prevent collision of function names)."
    )

set( USE_NPK_PREFIX OFF
    CACHE BOOL "Turn on to prevent collision of npk function names while linking."
    )

set( NPK_PREFIX NPK__
    CACHE STRING "Prefix for function names. (this will be used when you turn USE_NPK_PREFIX on)"
    )

set( DEV_MODE OFF
    CACHE BOOL "Build developer version library. Turn on if you want to build npk cli/gui tools."
    )

set( CASE_SENSITIVE OFF
    CACHE BOOL "Turn on to make entity names case-sensitive."
    )

set( BUILD_SHARED_LIBS OFF
    CACHE BOOL "Build libnpk as shared(like dylib, so or dll) library."
    )

enable_testing()

add_subdirectory( libnpk )
if( DEV_MODE )
    add_subdirectory( npk )
endif( DEV_MODE )
