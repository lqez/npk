cmake_minimum_required ( VERSION 2.6 )

project ( npk )

set( CMAKE_STATIC_LIBRARY_PREFIX "" )
set( CMAKE_SHARED_LIBRARY_PREFIX "" )

set_directory_properties(
	PROPERTY DEBUG_POSTFIX "d"
)

set( USE_ZLIB_PREFIX OFF
	CACHE BOOL "Turn on to use Z_PREFIX option."
	)

set( DEV_MODE OFF
	CACHE BOOL "Build dev. version library and tools."
	)

set( CASE_SENSITIVE OFF
	CACHE BOOL "Turn on to support case-sensitive entity names."
	)

set( BUILD_SHARED_LIBS OFF
	CACHE BOOL "Build shared(dylib,so,dll) libraries."
	)

set( BUILD_PYTHON_BINDING OFF
	CACHE STRING "Build python binding (needed on npk gui tool, need swig)."
	)

add_subdirectory( src )
#add_subdirectory( binding )
if( DEV_MODE )
	add_subdirectory( tool )
endif( DEV_MODE )
enable_testing()
add_subdirectory( tests )
