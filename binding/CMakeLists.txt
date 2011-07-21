if( BUILD_PYTHON_BINDING )
add_subdirectory( pynpk )
endif( BUILD_PYTHON_BINDING )

if( BUILD_OBJECTIVEC_BINDING )
add_subdirectory( objnpk )
endif( BUILD_OBJECTIVEC_BINDING )

if( BUILD_JNI_BINDING )
add_subdirectory( jvnpk )
endif( BUILD_JNI_BINDING )
