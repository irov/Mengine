include(ExternalProject)

ExternalProject_Add(glad_download PREFIX glad
		DOWNLOAD_COMMAND ""
		SOURCE_DIR ${THIRDPARTY_DIR}/glad

        UPDATE_COMMAND ${CMAKE_COMMAND} -E copy 
			${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists_glad.txt
			${THIRDPARTY_DIR}/glad/CMakeLists.txt
      
        CMAKE_ARGS 
            -DCMAKE_INSTALL_PREFIX=${THIRDPARTY_DIR}/glad/${CMAKE_GENERATOR}/${CMAKE_BUILD_TYPE}
            -DGLAD_API=gl=3.3
            -DGLAD_INSTALL=ON
    )

ExternalProject_Get_Property(glad_download INSTALL_DIR)
set(GLAD_INCLUDE_DIR ${INSTALL_DIR}/include)
set(GLAD_LIBRARY_DIR ${INSTALL_DIR}/lib)

add_library(glad STATIC IMPORTED)

set_target_properties(glad PROPERTIES IMPORTED_LOCATION ${GLAD_LIBRARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}glad${CMAKE_STATIC_LIBRARY_SUFFIX})