cmake_minimum_required ( VERSION 2.6 )

project ( npack )

set_directory_properties(
	PROPERTY DEBUG_POSTFIX "d"
)

set( DEV_MODE OFF
	CACHE BOOL "To build dev-lib or npack tools."
	)

set( BUILD_SHARED_LIBS OFF
	CACHE BOOL "Turn on if you want to build shared(dylib,so,dll) libraries."
	)

add_subdirectory( src )
add_subdirectory( tests )
if( DEV_MODE )
	add_subdirectory( tool )
endif( DEV_MODE )
