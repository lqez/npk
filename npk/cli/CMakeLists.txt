include_directories( ../../libnpk/include )

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

if( MSVC_IDE )
	foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
		string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
		set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "./" )
	endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )
endif( MSVC_IDE )

add_executable( npk ${SRCS} )
target_link_libraries( npk libnpk )

enable_testing()
add_subdirectory( tests )
