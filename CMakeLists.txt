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

set( DEV_MODE OFF
    CACHE BOOL "Build developer version library. Turn on if you want to build npk cli/gui tools."
    )

set( CASE_SENSITIVE OFF
    CACHE BOOL "Turn on to make entity names case-sensitive."
    )

set( BUILD_SHARED_LIBS OFF
    CACHE BOOL "Build libnpk as shared(like dylib, so or dll) library."
    )

#set( BUILD_PYTHON_BINDING OFF
#    CACHE STRING "Build Python binding (needed on npk gui tool, need swig)."
#    )

#set( BUILD_OBJECTIVEC_BINDING OFF
#    CACHE STRING "Build Objective-C binding."
#    )

#set( BUILD_JNI_BINDING OFF
#    CACHE STRING "Build JNI binding."
#    )

enable_testing()

add_subdirectory( libnpk )
add_subdirectory( binding )
if( DEV_MODE )
    add_subdirectory( npk )
endif( DEV_MODE )
