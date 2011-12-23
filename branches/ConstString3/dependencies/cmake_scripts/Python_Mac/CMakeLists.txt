CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

project(python)

SET( CMAKE_CURRENT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/Python )

INCLUDE( ../menge_macro.cmake )

MENGE_SET_PROJECT_DEFAULTS( python )

option(WITH_THREAD "compile in rudimentary thread support" OFF)
option(WITH_DOC_STRINGS "Enable if you want documentation strings in extension modules" OFF)
option(Py_USING_UNICODE "Enable unicode support" ON)
option(Py_DEBUG "Build debugging interpreter" OFF)
option(WITH_PYMALLOC "Define if you want to compile in Python-specific mallocs" ON)
option(HAVE_LARGEFILE_SUPPORT "Large file support " ON)

# this macro let's you decide whether you want to build the macro 
# and if yes, whether you want to build it statically or as module

if(WIN32)
  set(PY_POSIX_NAME nt)
else(WIN32)
  set(PY_POSIX_NAME posix)
endif(WIN32)

set( HAVE_CHROOT 0 )
set( HAVE_FSYNC 0 )
set( HAVE_WAIT3 0 )
set( HAVE_WAIT4 0 )
set( HAVE_DEVICE_MACROS 0 )

#file(MAKE_DIRECTORY ${python_BINARY_DIR}/Modules)
# for now just copy the config header on Windows, the checks may have to be adapted a bit
if(WIN32)
  configure_file(${CMAKE_SOURCE_DIR}/Python/PC/pyconfig.h ${CMAKE_SOURCE_DIR}/Python/pyconfig.h COPYONLY)
else(WIN32)
  configure_file(${CMAKE_SOURCE_DIR}/cmake_scripts/Python_Mac/pyconfig.h ${CMAKE_SOURCE_DIR}/Python/pyconfig.h COPYONLY)
endif(WIN32)

#configure_file(Modules/config.c.cmake ${CMAKE_CURRENT_BINARY_DIR}/Modules/modules-config.c @ONLY)
#file(WRITE ${python_BINARY_DIR}/Modules/Setup "#created by cmake, do not edit\n")
#configure_file(Modules/wininit.c.cmake ${CMAKE_CURRENT_BINARY_DIR}/Modules/wininit.c COPYONLY)

#if( WIN32 )
#	configure_file( ${CMAKE_SOURCE_DIR}/cmake_scripts/Python_2_7/pyconfig.h ${CMAKE_CURRENT_SOURCE_DIR}/PC/pyconfig.h COPYONLY)
#else( WIN32 )
#	configure_file( ${CMAKE_SOURCE_DIR}/cmake_scripts/Python_2_7/pyconfig.h.cmake ${CMAKE_CURRENT_SOURCE_DIR}/pyconfig.h)
#endif( WIN32 )

#configure_file( ${CMAKE_SOURCE_DIR}/cmake_scripts/Python_2_7/Modules/zipimport.c ${CMAKE_CURRENT_SOURCE_DIR}/Modules/zipimport.c COPYONLY)
#configure_file( ${CMAKE_SOURCE_DIR}/cmake_scripts/Python_2_7/Include/MengeStdioWrapper.h ${CMAKE_CURRENT_SOURCE_DIR}/Include/MengeStdioWrapper.h COPYONLY)

############################## 

               
set(PARSER_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/Parser/acceler.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Parser/grammar1.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Parser/listnode.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Parser/node.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Parser/parser.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Parser/parsetok.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Parser/bitset.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Parser/metagrammar.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Parser/firstsets.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Parser/grammar.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Parser/pgen.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Parser/myreadline.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Parser/tokenizer.c )

set(OBJECT_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/Objects/abstract.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/boolobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/bufferobject.c
				${CMAKE_CURRENT_SOURCE_DIR}/Objects/bytes_methods.c
				${CMAKE_CURRENT_SOURCE_DIR}/Objects/bytearrayobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/cellobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/classobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/cobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/codeobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/complexobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/descrobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/enumobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/exceptions.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/genobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/fileobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/floatobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/frameobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/funcobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/intobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/iterobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/listobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/longobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/dictobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/methodobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/moduleobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/object.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/obmalloc.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/rangeobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/setobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/sliceobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/stringobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/structseq.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/tupleobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/typeobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/weakrefobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/capsule.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/memoryobject.c
                
                
   )
   
if(Py_USING_UNICODE)
   set(OBJECT_SRCS ${OBJECT_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/Objects/unicodeobject.c ${CMAKE_CURRENT_SOURCE_DIR}/Objects/unicodectype.c)
endif(Py_USING_UNICODE)


set(PYTHON_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/Python/_warnings.c
				${CMAKE_CURRENT_SOURCE_DIR}/Python/Python-ast.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/asdl.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/ast.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/bltinmodule.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/ceval.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/compile.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/codecs.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/errors.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/frozen.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/frozenmain.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/future.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/getargs.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/getcompiler.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/getcopyright.c 
#                ${CMAKE_CURRENT_SOURCE_DIR}/Python/getmtime.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/getplatform.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/getversion.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/graminit.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/import.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/importdl.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/marshal.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/modsupport.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/mystrtoul.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/mysnprintf.c 
				${CMAKE_CURRENT_SOURCE_DIR}/Python/peephole.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/pyarena.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/pyfpe.c 
				${CMAKE_CURRENT_SOURCE_DIR}/Python/pymath.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/pystate.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/pythonrun.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/structmember.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/symtable.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/sysmodule.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/traceback.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/getopt.c 
				${CMAKE_CURRENT_SOURCE_DIR}/Python/pystrcmp.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/pystrtod.c 
				${CMAKE_CURRENT_SOURCE_DIR}/Python/formatter_unicode.c
				${CMAKE_CURRENT_SOURCE_DIR}/Python/formatter_string.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/pyctype.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/dtoa.c
                
                #${MACHDEP_SRCS}
                ${THREAD_SRC}
                ${DYNLOAD_SRC}
                )

set(MODULE_SRCS 
                #${CMAKE_CURRENT_BINARY_DIR}/Modules/modules-config.c 
                #${CMAKE_CURRENT_SOURCE_DIR}/Modules/getpath.c 
                #${CMAKE_CURRENT_SOURCE_DIR}/Modules/main.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Modules/gcmodule.c
				
                #${CMAKE_CURRENT_SOURCE_DIR}/Modules/timemodule.c
				#${CMAKE_CURRENT_SOURCE_DIR}/Modules/cStringIO.c
				#${CMAKE_CURRENT_SOURCE_DIR}/Modules/arraymodule.c
				#${CMAKE_CURRENT_SOURCE_DIR}/Modules/datetimemodule.c		
                ${CMAKE_CURRENT_SOURCE_DIR}/Modules/signalmodule.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Modules/errnomodule.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Modules/_sre.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Modules/_codecsmodule.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Modules/zipimport.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Modules/symtablemodule.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Modules/xxsubtype.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Modules/_struct.c                
				)
				
if( NOT WIN32 )
	set( MODULE_SRCS ${MODULE_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/Modules/getpath.c )
else( NOT WIN32 )
	set( MODULE_SRCS ${MODULE_SRCS} 
	${CMAKE_CURRENT_BINARY_DIR}/Modules/wininit.c
	${CMAKE_CURRENT_SOURCE_DIR}/PC/getpathp.c	)
endif( NOT WIN32 )

#set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/Python/getplatform.c PROPERTIES COMPILE_FLAGS -DPLATFORM=\\"${PY_PLATFORM}\\")
#set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/Modules/getpath.c PROPERTIES COMPILE_FLAGS  "-DPREFIX=\\\"${CMAKE_INSTALL_PREFIX}\\\" -DEXEC_PREFIX=\\\"${CMAKE_INSTALL_PREFIX}\\\" -DVERSION=\\\"2.6\\\" -DVPATH=\\\"..\\\"  -DPYTHONPATH=\\\":plat-linux2:lib-tk\\\" " )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Include ${CMAKE_CURRENT_SOURCE_DIR})

#if(WIN32)
#  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/PC ${CMAKE_CURRENT_SOURCE_DIR}/Python)
#  if(MSVC)
#	  add_definitions(-DWIN32 -D_WINDOWS -D_MSC_VER=1400)
#  else(MSVC)
#	  add_definitions(-D_WINDOWS)
#  endif(MSVC)
#endif(WIN32)

add_definitions(-DPy_NO_ENABLE_SHARED -DPy_BUILD_CORE)

#if(NOT HAVE_HYPOT)
 # add_definitions(-DWITHOUT_COMPLEX)
#endif(NOT HAVE_HYPOT)

# add_library(${python_LIB_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/Modules/getbuildinfo.c ${CMAKE_CURRENT_SOURCE_DIR}/Modules/_typesmodule.c
              # ${PARSER_SRCS}
              # ${OBJECT_SRCS}
              # ${PYTHON_SRCS}
              # ${MODULE_SRCS}
             #${SIGNAL_SRCS}
              # )
			  
add_library( ${python_LIB_NAME} STATIC
			  ${CMAKE_CURRENT_SOURCE_DIR}/Modules/getbuildinfo.c 
#			  ${CMAKE_CURRENT_SOURCE_DIR}/Modules/_typesmodule.c
              ${PARSER_SRCS}
              ${OBJECT_SRCS}
              ${PYTHON_SRCS}
              ${MODULE_SRCS} )

#target_link_libraries(${python_LIB_NAME} ${extraLibs})
#set_target_properties(pythonLib PROPERTIES OUTPUT_NAME ${python_LIB_NAME})

#add_executable(python Modules/python.c )
#target_link_libraries(python pythonLib)

#install(TARGETS pythonLib DESTINATION lib)
#install(TARGETS python DESTINATION bin)
#file(GLOB pyHeaders ${Python_SOURCE_DIR}/Include/*.h )
#install(FILES ${Python_BINARY_DIR}/pyconfig.h ${pyHeaders} DESTINATION include/python${PY_MAJOR}.${PY_MINOR})
