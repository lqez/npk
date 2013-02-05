include_directories( ../include )

set( ITEMS
    libnpk_open.cpp
    libnpk_read_entity.cpp
    libnpk_read_entity_partial.cpp
    libnpk_case.cpp
    libnpk_using_fd.cpp
)

if( DEV_MODE )
    set( ITEMS
        ${ITEMS}
        libnpk_write.cpp
        libnpk_xxtea.cpp
        libnpk_durability.cpp
        libnpk_export.cpp
        libnpk_hash.cpp
        libnpk_remove.cpp
        libnpk_streamable.cpp
    )
endif( DEV_MODE )

create_test_sourcelist( TESTS
    tests.cpp
    ${ITEMS}
)

add_executable( libnpkTests ${TESTS} )
target_link_libraries( libnpkTests libnpkStatic )

remove( TESTS tests.cpp )

# push test cases into the target
foreach( test ${TESTS} )
    get_filename_component( TName ${test} NAME_WE )
    add_test( ${TName} libnpkTests ${TName} )
endforeach( test )

# copy test resources into test directory
file( GLOB files "testres/[^.]*" )
foreach( file ${files} )
    add_custom_command( 
        TARGET libnpkTests
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} ARGS -E copy "${file}" "${CMAKE_CURRENT_BINARY_DIR}/"
    )
endforeach( file ${TEST_RESOURCES} )
