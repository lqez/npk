cmake_minimum_required ( VERSION 2.6 )

project ( npk )

if( WIN32 )
	set( LIB_NAME libnpk )
else( WIN32 )
	set( LIB_NAME npk )
endif( WIN32 )

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
if( DEV_MODE )
	add_subdirectory( tool )
endif( DEV_MODE )
enable_testing()
add_subdirectory( tests )
