include_directories( ../external/tea )
include_directories( ../external/zlib )
include_directories( ../include )

set( TEA_SRCS
	../external/tea/tea.c
	)

set( ZLIB_UNCOMP_SRCS
	../external/zlib/adler32.c
	../external/zlib/crc32.c
	../external/zlib/infback.c
	../external/zlib/inffast.c
	../external/zlib/inflate.c
	../external/zlib/inftrees.c
	../external/zlib/uncompr.c
	../external/zlib/zutil.c
	)

set( ZLIB_COMP_SRCS
	../external/zlib/deflate.c
	../external/zlib/compress.c
	../external/zlib/trees.c
	)

set( SRCS
	npk.c
	npk_common.c
	)

set( SRCS_DEV
	npk_dev.c
	)

if( WIN32 )
	if( BUILD_SHARED_LIBS )
		add_definitions( -DNPK_DLL -DNPK_EXPORTS )
	endif( BUILD_SHARED_LIBS )
endif( WIN32 )

if( DEV_MODE )
	add_definitions( -DZ_PREFIX -DNPK_DEV )
	add_library( ${LIB_NAME} ${SRCS} ${SRCS_DEV} ${TEA_SRCS} ${ZLIB_COMP_SRCS} ${ZLIB_UNCOMP_SRCS} )
else( DEV_MODE )
	add_definitions( -DZ_PREFIX )
	add_library( ${LIB_NAME} ${SRCS} ${TEA_SRCS} ${ZLIB_UNCOMP_SRCS} )
endif( DEV_MODE )
