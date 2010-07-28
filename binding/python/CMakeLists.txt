find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

find_package(PythonLibs)
include_directories(${PYTHON_INCLUDE_PATH})

include_directories(${CMAKE_SOURCE_DIR}/binding)
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/external/tea)
include_directories(${CMAKE_SOURCE_DIR}/external/zlib)

set( TEA_SRCS
	../../external/tea/tea.c
	)

set( ZLIB_UNCOMP_SRCS
	../../external/zlib/adler32.c
	../../external/zlib/crc32.c
	../../external/zlib/infback.c
	../../external/zlib/inffast.c
	../../external/zlib/inflate.c
	../../external/zlib/inftrees.c
	../../external/zlib/uncompr.c
	../../external/zlib/zutil.c
	)

set( ZLIB_COMP_SRCS
	../../external/zlib/deflate.c
	../../external/zlib/compress.c
	../../external/zlib/trees.c
	)

set( SRCS
	../../src/npk.c
	../../src/npk_common.c
	)

set( SRCS_DEV
	../../src/npk_dev.c
	)

if( DEV_MODE )
	add_definitions( -DNPK_DEV )
	swig_add_module( npk python ../npk_dev.i ${SRCS} ${SRCS_DEV} ${TEA_SRCS} ${ZLIB_COMP_SRCS} ${ZLIB_UNCOMP_SRCS} )
else( DEV_MODE )
	swig_add_module( npk python ../npk.i ${SRCS} ${TEA_SRCS} ${ZLIB_UNCOMP_SRCS} )
endif( DEV_MODE )
swig_link_libraries(npk ${PYTHON_LIBRARIES})
