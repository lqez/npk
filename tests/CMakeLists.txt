include_directories( ../include )

set( SAMPLES
	${SAMPLES}
	sample.txt
	${PROJECT_SOURCE_DIR}/tests/*.npk
	${PROJECT_SOURCE_DIR}/tests/*.bin
	)

set( ITEMS
	${ITEMS}
	npk_open.cpp
	npk_read_entity.cpp
	npk_case.cpp
	npk_using_fd.cpp
	)

if( DEV_MODE )
	set( ITEMS
		${ITEMS}
		npk_write.cpp
		npk_durability.cpp
		npk_export.cpp
		npk_hash.cpp
		)
endif( DEV_MODE )

create_test_sourcelist( TESTS
	tests.cpp
	${ITEMS}
	)

add_executable( tests ${TESTS} )
target_link_libraries( tests libnpk )

remove( TESTS tests.cpp )

# push test cases into the target
foreach( test ${TESTS} )
	get_filename_component( TName ${test} NAME_WE )
	add_test( ${TName} tests ${TName} )
endforeach( test )

# copy test resources into test directory
file( GLOB TEST_RESOURCES
	${SAMPLES}
	)
foreach( file ${TEST_RESOURCES} )
	add_custom_command( 
		TARGET tests
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} ARGS -E copy ${file} ${PROJECT_BINARY_DIR}/tests
		)
endforeach( file ${TEST_RESOURCES} )
