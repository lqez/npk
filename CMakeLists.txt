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

set( DEV_MODE ON
    CACHE BOOL "Build developer version library. Should be turned on to build npk tool."
    )

set( BUILD_NPK ON
    CACHE BOOL "Build command line executable tool 'npk'."
    )

set( CASE_SENSITIVE OFF
    CACHE BOOL "Turn on to make entity names case-sensitive."
    )

enable_testing()

add_subdirectory( libnpk )
if( BUILD_NPK AND DEV_MODE )
    add_subdirectory( npk )
endif( BUILD_NPK AND DEV_MODE )
