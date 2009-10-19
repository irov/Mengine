
cmake_minimum_required(VERSION 2.4.5 FATAL_ERROR)

project(python)

SET( CMAKE_CURRENT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/../python )

INCLUDE( ../menge_macro.cmake )

MENGE_SET_PROJECT_DEFAULTS( python )

#patch
if( MINGW )
	configure_file( ${CMAKE_SOURCE_DIR}/Python/Python/dynload_win.c.mingw ${CMAKE_CURRENT_SOURCE_DIR}/Python/dynload_win.c COPYONLY)
	configure_file( ${CMAKE_SOURCE_DIR}/Python/Modules/posixmodule.c.mingw ${CMAKE_CURRENT_SOURCE_DIR}/Modules/posixmodule.c COPYONLY)
endif( MINGW )
if( MSVC )
	configure_file( ${CMAKE_SOURCE_DIR}/Python/Python/dynload_win.c.msvc8 ${CMAKE_CURRENT_SOURCE_DIR}/Python/dynload_win.c COPYONLY)
	configure_file( ${CMAKE_SOURCE_DIR}/Python/Modules/posixmodule.c.msvc8 ${CMAKE_CURRENT_SOURCE_DIR}/Modules/posixmodule.c COPYONLY)
endif( MSVC )

configure_file( ${CMAKE_SOURCE_DIR}/Python/pyconfig.h ${CMAKE_CURRENT_SOURCE_DIR}/PC/pyconfig.h COPYONLY)
configure_file( ${CMAKE_SOURCE_DIR}/Python/Modules/zipimport.c ${CMAKE_CURRENT_SOURCE_DIR}/Modules/zipimport.c COPYONLY)
configure_file( ${CMAKE_SOURCE_DIR}/Python/Include/MengeStdioWrapper.h ${CMAKE_CURRENT_SOURCE_DIR}/Include/MengeStdioWrapper.h COPYONLY)

option(WITH_THREAD "compile in rudimentary thread support" 1)
option(WITH_DOC_STRINGS "Enable if you want documentation strings in extension modules" 1)
option(Py_USING_UNICODE "Enable unicode support" 1)
option(Py_DEBUG "Build debugging interpreter" 0)
option(WITH_PYMALLOC "Define if you want to compile in Python-specific mallocs" 1)
option(HAVE_LARGEFILE_SUPPORT "Large file support "1)

include(ConfigureChecks.cmake)

set(extraLibs)

set(THREAD_SRC)
if(WITH_THREAD)
  if(HAVE_LIBPTHREAD)
    set(THREAD_SRC ${CMAKE_CURRENT_SOURCE_DIR}/Python/thread.c)
    set(_REENTRANT 1)
    set(extraLibs ${extraLibs} ${HAVE_LIBPTHREAD})
  endif(HAVE_LIBPTHREAD)
  if(WIN32)
    set(THREAD_SRC ${CMAKE_CURRENT_SOURCE_DIR}/Python/thread.c)
  endif(WIN32)
endif(WITH_THREAD)

if(HAVE_LIBM)
  set(extraLibs ${extraLibs} ${HAVE_LIBM})
endif(HAVE_LIBM)

if(HAVE_LIBUTIL)
  set(extraLibs ${extraLibs} ${HAVE_LIBUTIL})
endif(HAVE_LIBUTIL)

set(DYNLOAD_SRC)
if(HAVE_DYNAMIC_LOADING)
  if(HAVE_LIBDL)
    set(extraLibs ${extraLibs} ${HAVE_LIBDL})
    set(DYNLOAD_SRC ${DYNLOAD_SRC} ${CMAKE_CURRENT_SOURCE_DIR}/Python/dynload_shlib.c)
  endif(HAVE_LIBDL)
  if(WIN32)
    set(DYNLOAD_SRC ${DYNLOAD_SRC} ${CMAKE_CURRENT_SOURCE_DIR}/Python/dynload_win.c ${CMAKE_CURRENT_SOURCE_DIR}/PC/import_nt.c ${CMAKE_CURRENT_SOURCE_DIR}/PC/dl_nt.c ${CMAKE_CURRENT_SOURCE_DIR}/PC/getpathp.c)
  endif(WIN32)
endif(HAVE_DYNAMIC_LOADING)


set(MACHDEP_SRCS ) #conf 

set(SIGNAL_SRCS )    # configure

set(MOD_SRCS )

# this macro let's you decide whether you want to build the macro 
# and if yes, whether you want to build it statically or as module
macro(ADD_PYTHON_MODULE _NAME _STATIC_SRCS)
  option(MODULE_${_NAME}_ENABLE "Add module ${_NAME}" TRUE)

  if(MODULE_${_NAME}_ENABLE)
    option(MODULE_${_NAME}_SHARED "Add module ${_NAME} shared" FALSE)
    if(MODULE_${_NAME}_SHARED)
      message(STATUS "NOT Adding ${_NAME} as shared module (not yet implemented)")
    else(MODULE_${_NAME}_SHARED)
      message(STATUS "Adding ${_NAME} as static module")
      foreach (_current_FILE ${ARGN})
        set(${_STATIC_SRCS} ${${_STATIC_SRCS}} ${_current_FILE})
      endforeach (_current_FILE ${ARGN})
      set(PY_STATIC_MODULES_INIT_FUNCTIONS "${PY_STATIC_MODULES_INIT_FUNCTIONS}extern void init${_NAME}(void);\n")
      set(PY_STATIC_MODULES_INITTAB        "${PY_STATIC_MODULES_INITTAB}  {\"${_NAME}\", init${_NAME}},\n")
    endif(MODULE_${_NAME}_SHARED)
  endif(MODULE_${_NAME}_ENABLE)
endmacro(ADD_PYTHON_MODULE)

if(WITH_THREAD)
  add_python_module(thread  MOD_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/Modules/threadmodule.c)
endif(WITH_THREAD)

if(WIN32)
  set(PY_POSIX_NAME nt)
else(WIN32)
  set(PY_POSIX_NAME posix)
endif(WIN32)

add_python_module(${PY_POSIX_NAME}  MOD_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/Modules/posixmodule.c)
add_python_module(signal    MOD_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/Modules/signalmodule.c)
add_python_module(errno     MOD_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/Modules/errnomodule.c)
#add_python_module(pwd       MOD_SRCS ${TRUE_DIR}/Modules/pwdmodule.c)
add_python_module(_sre      MOD_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/Modules/_sre.c)
add_python_module(_codecs   MOD_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/Modules/_codecsmodule.c)
add_python_module(zipimport MOD_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/Modules/zipimport.c)
add_python_module(_symtable MOD_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/Modules/symtablemodule.c)
add_python_module(xxsubtype MOD_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/Modules/xxsubtype.c)


file(MAKE_DIRECTORY ${python_BINARY_DIR}/Modules)
# for now just copy the config header on Windows, the checks may have to be adapted a bit
#if(WIN32)
#  configure_file(PC/pyconfig.h ${python_BINARY_DIR}/pyconfig.h COPYONLY)
#else(WIN32)
#  configure_file(pyconfig.h.cmake ${python_BINARY_DIR}/pyconfig.h @ONLY)
#endif(WIN32)
#configure_file(pyconfig.h.cmake pyconfig.h @ONLY)

configure_file(Modules/config.c.cmake ${CMAKE_CURRENT_BINARY_DIR}/Modules/modules-config.c @ONLY)
file(WRITE ${python_BINARY_DIR}/Modules/Setup "#created by cmake, do not edit\n")

############################## 

set(P_SRCS      ${CMAKE_CURRENT_SOURCE_DIR}/Parser/acceler.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Parser/grammar1.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Parser/listnode.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Parser/node.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Parser/parser.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Parser/parsetok.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Parser/bitset.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Parser/metagrammar.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Parser/firstsets.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Parser/grammar.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Parser/pgen.c )

set(PARSER_SRCS ${P_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/Parser/myreadline.c ${CMAKE_CURRENT_SOURCE_DIR}/Parser/tokenizer.c )

set(OBJECT_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/Objects/abstract.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/boolobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/bufferobject.c
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
   )

if(Py_USING_UNICODE)
   set(OBJECT_SRCS ${OBJECT_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/Objects/unicodeobject.c ${CMAKE_CURRENT_SOURCE_DIR}/Objects/unicodectype.c)
endif(Py_USING_UNICODE)

set(PYTHON_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/Python/Python-ast.c 
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
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/getmtime.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/getplatform.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/getversion.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/graminit.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/import.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/importdl.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/marshal.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/modsupport.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/mystrtoul.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/mysnprintf.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/pyarena.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/pyfpe.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/pystate.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/pythonrun.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/structmember.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/symtable.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/sysmodule.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/traceback.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/getopt.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/pystrtod.c 
                #${MACHDEP_SRCS}
                ${THREAD_SRC}
                ${DYNLOAD_SRC}
                )

set(MODULE_SRCS ${CMAKE_CURRENT_BINARY_DIR}/Modules/modules-config.c 
               # Modules/getpath.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Modules/main.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Modules/gcmodule.c  
                ${MOD_SRCS} )

set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/Python/getplatform.c PROPERTIES COMPILE_FLAGS -DPLATFORM=\\"${PY_PLATFORM}\\")
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/Modules/getpath.c PROPERTIES COMPILE_FLAGS  "-DPREFIX=\\\"${CMAKE_INSTALL_PREFIX}\\\" -DEXEC_PREFIX=\\\"${CMAKE_INSTALL_PREFIX}\\\" -DVERSION=\\\"2.5\\\" -DVPATH=\\\"..\\\"  -DPYTHONPATH=\\\":plat-linux2:lib-tk\\\" " )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Include ${python_SOURCE_DIR})
if(WIN32)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/PC ${CMAKE_CURRENT_SOURCE_DIR}/Python)
  if(MSVC)
	  add_definitions(-DWIN32 -D_WINDOWS -D_MSC_VER=1400)
  else(MSVC)
	  add_definitions(-D_WINDOWS)
  endif(MSVC)
endif(WIN32)

#add_definitions(-DPy_NO_ENABLE_SHARED -DPy_BUILD_CORE)

if(NOT HAVE_HYPOT)
  add_definitions(-DWITHOUT_COMPLEX)
endif(NOT HAVE_HYPOT)

# add_library(${python_LIB_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/Modules/getbuildinfo.c ${CMAKE_CURRENT_SOURCE_DIR}/Modules/_typesmodule.c
              # ${PARSER_SRCS}
              # ${OBJECT_SRCS}
              # ${PYTHON_SRCS}
              # ${MODULE_SRCS}
             #${SIGNAL_SRCS}
              # )
			  
add_library( ${python_LIB_NAME} STATIC
			  ${CMAKE_CURRENT_SOURCE_DIR}/Modules/getbuildinfo.c 
			  ${CMAKE_CURRENT_SOURCE_DIR}/Modules/_typesmodule.c
              ${PARSER_SRCS}
              ${OBJECT_SRCS}
              ${PYTHON_SRCS}
              ${MODULE_SRCS} )

target_link_libraries(${python_LIB_NAME} ${extraLibs})
#set_target_properties(pythonLib PROPERTIES OUTPUT_NAME ${python_LIB_NAME})

#add_executable(python Modules/python.c )
#target_link_libraries(python pythonLib)

#install(TARGETS pythonLib DESTINATION lib)
#install(TARGETS python DESTINATION bin)
#file(GLOB pyHeaders ${Python_SOURCE_DIR}/Include/*.h )
#install(FILES ${Python_BINARY_DIR}/pyconfig.h ${pyHeaders} DESTINATION include/python${PY_MAJOR}.${PY_MINOR})
