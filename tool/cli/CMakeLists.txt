include_directories( ../../include )

if( WIN32 )
	set( SRCS
		npack_cli.cpp
		helper_dirent.c
		)
	if( BUILD_SHARED_LIBS )
		add_definitions( -DNPACK_DLL )
	endif( BUILD_SHARED_LIBS )
else( WIN32 )
	set( SRCS
		npack_cli.cpp
		)
endif( WIN32 )

add_executable( npack ${SRCS} )
target_link_libraries( npack libnpack )
