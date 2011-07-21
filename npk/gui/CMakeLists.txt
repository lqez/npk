include_directories( ../../include )

add_definitions( -D__WXMSW__ )

if( CASE_SESITIVE )
	add_definitions( -DNPK_CASESENSITIVE )
endif( CASE_SESITIVE )

set( SRCS
	npk_gui.cpp
	)


if( WIN32 )
	add_executable( npkx WIN32 ${SRCS} )
	include_directories( SYSTEM "%WXWIN%/include" )
	include_directories( SYSTEM "%WXWIN%/include/msvc" )
	target_link_libraries( npkx libnpk comctl32 rpcrt4 winmm advapi32 wsock32 )
else( WIN32 )
	add_executable( npkx ${SRCS} )
	target_link_libraries( npkx libnpk )
endif( WIN32 )
