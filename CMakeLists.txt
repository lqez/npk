cmake_minimum_required ( VERSION 2.6 )

project ( npk )

set_directory_properties(
	PROPERTY DEBUG_POSTFIX "d"
)

set( DEV_MODE OFF
	CACHE BOOL "Build dev. version library and tools."
	)

set( BUILD_SHARED_LIBS OFF
	CACHE BOOL "Build shared(dylib,so,dll) libraries."
	)

add_subdirectory( src )
add_subdirectory( tests )
if( DEV_MODE )
	add_subdirectory( tool )
endif( DEV_MODE )
