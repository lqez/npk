include_directories( ../../include )

if( CASE_SESITIVE )
	add_definitions( -DNPK_CASESENSITIVE )
endif( CASE_SESITIVE )

if( WIN32 )
	set( SRCS
		npk_cli.cpp
		helper_dirent.c
		)
	if( BUILD_SHARED_LIBS )
		add_definitions( -DNPK_DLL )
	endif( BUILD_SHARED_LIBS )
else( WIN32 )
	set( SRCS
		npk_cli.cpp
		)
endif( WIN32 )

add_executable( npk ${SRCS} )
target_link_libraries( npk libnpk )
