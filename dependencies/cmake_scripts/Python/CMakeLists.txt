
cmake_minimum_required(VERSION 2.4.5 FATAL_ERROR)

project(Python)

SET(TRUE_DIR ../../Python)
SET(Python_SOURCE_DIR ${TRUE_DIR})

INCLUDE( ../menge_macro.cmake )

option(WITH_TSC "profile with the Pentium timestamp counter" 0)
option(ENABLE_IPV6 "Enable IP v6" 0)
option(WITH_THREAD "compile in rudimentary thread support" 1)
option(WITH_DOC_STRINGS "Enable if you want documentation strings in extension modules" 1)
option(Py_USING_UNICODE "Enable unicode support" 1)
option(Py_DEBUG "Build debugging interpreter" 0)
option(WITH_PYMALLOC "Define if you want to compile in Python-specific mallocs" 1)
option(HAVE_LARGEFILE_SUPPORT "Large file support "1)

set(PY_MAJOR 2)
set(PY_MINOR 5)

include(ConfigureChecks.cmake)

set(extraLibs)

set(THREAD_SRC)
if(WITH_THREAD)
  if(HAVE_LIBPTHREAD)
    set(THREAD_SRC ${TRUE_DIR}/Python/thread.c)
    set(_REENTRANT 1)
    set(extraLibs ${extraLibs} ${HAVE_LIBPTHREAD})
  endif(HAVE_LIBPTHREAD)
  if(WIN32)
    set(THREAD_SRC ${TRUE_DIR}/Python/thread.c)
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
    set(DYNLOAD_SRC ${DYNLOAD_SRC} ${TRUE_DIR}/Python/dynload_shlib.c)
  endif(HAVE_LIBDL)
  if(WIN32)
    set(DYNLOAD_SRC ${DYNLOAD_SRC} ${TRUE_DIR}/Python/dynload_win.c ${TRUE_DIR}/PC/import_nt.c ${TRUE_DIR}/PC/dl_nt.c ${TRUE_DIR}/PC/getpathp.c)
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
  add_python_module(thread  MOD_SRCS ${TRUE_DIR}/Modules/threadmodule.c)
endif(WITH_THREAD)

if(WIN32)
  set(PY_POSIX_NAME nt)
else(WIN32)
  set(PY_POSIX_NAME posix)
endif(WIN32)

add_python_module(${PY_POSIX_NAME}  MOD_SRCS ${TRUE_DIR}/Modules/posixmodule.c)
add_python_module(signal    MOD_SRCS ${TRUE_DIR}/Modules/signalmodule.c)
add_python_module(errno     MOD_SRCS ${TRUE_DIR}/Modules/errnomodule.c)
#add_python_module(pwd       MOD_SRCS ${TRUE_DIR}/Modules/pwdmodule.c)
add_python_module(_sre      MOD_SRCS ${TRUE_DIR}/Modules/_sre.c)
add_python_module(_codecs   MOD_SRCS ${TRUE_DIR}/Modules/_codecsmodule.c)
add_python_module(zipimport MOD_SRCS ${TRUE_DIR}/Modules/zipimport.c)
add_python_module(_symtable MOD_SRCS ${TRUE_DIR}/Modules/symtablemodule.c)
add_python_module(xxsubtype MOD_SRCS ${TRUE_DIR}/Modules/xxsubtype.c)


file(MAKE_DIRECTORY ${Python_BINARY_DIR}/Modules)
# for now just copy the config header on Windows, the checks may have to be adapted a bit
#if(WIN32)
#  configure_file(PC/pyconfig.h ${Python_BINARY_DIR}/pyconfig.h COPYONLY)
#else(WIN32)
#  configure_file(pyconfig.h.cmake ${Python_BINARY_DIR}/pyconfig.h @ONLY)
#endif(WIN32)
configure_file(Modules/config.c.cmake ${CMAKE_CURRENT_BINARY_DIR}/Modules/modules-config.c @ONLY)
file(WRITE ${Python_BINARY_DIR}/Modules/Setup "#created by cmake, do not edit\n")

############################## 

set(P_SRCS      ${TRUE_DIR}/Parser/acceler.c
                ${TRUE_DIR}/Parser/grammar1.c
                ${TRUE_DIR}/Parser/listnode.c
                ${TRUE_DIR}/Parser/node.c
                ${TRUE_DIR}/Parser/parser.c
                ${TRUE_DIR}/Parser/parsetok.c
                ${TRUE_DIR}/Parser/bitset.c
                ${TRUE_DIR}/Parser/metagrammar.c
                ${TRUE_DIR}/Parser/firstsets.c
                ${TRUE_DIR}/Parser/grammar.c
                ${TRUE_DIR}/Parser/pgen.c )

set(PARSER_SRCS ${P_SRCS} ${TRUE_DIR}/Parser/myreadline.c ${TRUE_DIR}/Parser/tokenizer.c )

set(OBJECT_SRCS ${TRUE_DIR}/Objects/abstract.c
                ${TRUE_DIR}/Objects/boolobject.c
                ${TRUE_DIR}/Objects/bufferobject.c
                ${TRUE_DIR}/Objects/cellobject.c
                ${TRUE_DIR}/Objects/classobject.c
                ${TRUE_DIR}/Objects/cobject.c
                ${TRUE_DIR}/Objects/codeobject.c
                ${TRUE_DIR}/Objects/complexobject.c
                ${TRUE_DIR}/Objects/descrobject.c
                ${TRUE_DIR}/Objects/enumobject.c
                ${TRUE_DIR}/Objects/exceptions.c
                ${TRUE_DIR}/Objects/genobject.c
                ${TRUE_DIR}/Objects/fileobject.c
                ${TRUE_DIR}/Objects/floatobject.c
                ${TRUE_DIR}/Objects/frameobject.c
                ${TRUE_DIR}/Objects/funcobject.c
                ${TRUE_DIR}/Objects/intobject.c
                ${TRUE_DIR}/Objects/iterobject.c
                ${TRUE_DIR}/Objects/listobject.c
                ${TRUE_DIR}/Objects/longobject.c
                ${TRUE_DIR}/Objects/dictobject.c
                ${TRUE_DIR}/Objects/methodobject.c
                ${TRUE_DIR}/Objects/moduleobject.c
                ${TRUE_DIR}/Objects/object.c
                ${TRUE_DIR}/Objects/obmalloc.c
                ${TRUE_DIR}/Objects/rangeobject.c
                ${TRUE_DIR}/Objects/setobject.c
                ${TRUE_DIR}/Objects/sliceobject.c
                ${TRUE_DIR}/Objects/stringobject.c
                ${TRUE_DIR}/Objects/structseq.c
                ${TRUE_DIR}/Objects/tupleobject.c
                ${TRUE_DIR}/Objects/typeobject.c
                ${TRUE_DIR}/Objects/weakrefobject.c
   )

if(Py_USING_UNICODE)
   set(OBJECT_SRCS ${OBJECT_SRCS} ${TRUE_DIR}/Objects/unicodeobject.c ${TRUE_DIR}/Objects/unicodectype.c)
endif(Py_USING_UNICODE)

set(PYTHON_SRCS ${TRUE_DIR}/Python/Python-ast.c 
                ${TRUE_DIR}/Python/asdl.c 
                ${TRUE_DIR}/Python/ast.c 
                ${TRUE_DIR}/Python/bltinmodule.c 
                ${TRUE_DIR}/Python/ceval.c 
                ${TRUE_DIR}/Python/compile.c 
                ${TRUE_DIR}/Python/codecs.c 
                ${TRUE_DIR}/Python/errors.c 
                ${TRUE_DIR}/Python/frozen.c 
                ${TRUE_DIR}/Python/frozenmain.c 
                ${TRUE_DIR}/Python/future.c 
                ${TRUE_DIR}/Python/getargs.c 
                ${TRUE_DIR}/Python/getcompiler.c 
                ${TRUE_DIR}/Python/getcopyright.c 
                ${TRUE_DIR}/Python/getmtime.c 
                ${TRUE_DIR}/Python/getplatform.c 
                ${TRUE_DIR}/Python/getversion.c 
                ${TRUE_DIR}/Python/graminit.c 
                ${TRUE_DIR}/Python/import.c 
                ${TRUE_DIR}/Python/importdl.c 
                ${TRUE_DIR}/Python/marshal.c 
                ${TRUE_DIR}/Python/modsupport.c 
                ${TRUE_DIR}/Python/mystrtoul.c 
                ${TRUE_DIR}/Python/mysnprintf.c 
                ${TRUE_DIR}/Python/pyarena.c 
                ${TRUE_DIR}/Python/pyfpe.c 
                ${TRUE_DIR}/Python/pystate.c 
                ${TRUE_DIR}/Python/pythonrun.c 
                ${TRUE_DIR}/Python/structmember.c 
                ${TRUE_DIR}/Python/symtable.c 
                ${TRUE_DIR}/Python/sysmodule.c 
                ${TRUE_DIR}/Python/traceback.c 
                ${TRUE_DIR}/Python/getopt.c 
                ${TRUE_DIR}/Python/pystrtod.c 
                #${MACHDEP_SRCS}
                ${THREAD_SRC}
                ${DYNLOAD_SRC}
                )

set(MODULE_SRCS ${CMAKE_CURRENT_BINARY_DIR}/Modules/modules-config.c 
               # Modules/getpath.c 
                ${TRUE_DIR}/Modules/main.c 
                ${TRUE_DIR}/Modules/gcmodule.c  
                ${MOD_SRCS} )

set_source_files_properties(${TRUE_DIR}/Python/getplatform.c PROPERTIES COMPILE_FLAGS -DPLATFORM=\\"${PY_PLATFORM}\\")
set_source_files_properties(${TRUE_DIR}/Modules/getpath.c PROPERTIES COMPILE_FLAGS  "-DPREFIX=\\\"${CMAKE_INSTALL_PREFIX}\\\" -DEXEC_PREFIX=\\\"${CMAKE_INSTALL_PREFIX}\\\" -DVERSION=\\\"2.5\\\" -DVPATH=\\\"..\\\"  -DPYTHONPATH=\\\":plat-linux2:lib-tk\\\" " )

include_directories(${TRUE_DIR}/Include ${Python_SOURCE_DIR})
if(WIN32)
  include_directories(${TRUE_DIR}/PC ${TRUE_DIR}/Python)
  add_definitions(-DWIN32 -D_WINDOWS -D_MSC_VER=1400)
endif(WIN32)

add_definitions(-DPy_BUILD_CORE)

if(NOT HAVE_HYPOT)
  add_definitions(-DWITHOUT_COMPLEX)
endif(NOT HAVE_HYPOT)

if(MSVC)
 set(python_LIB_NAME lib)
endif(MSVC)
set( python_LIB_NAME ${python_LIB_NAME}python_static )

MENGINE_ADD_LIBRARY(${python_LIB_NAME} ${TRUE_DIR}/Modules/getbuildinfo.c ${TRUE_DIR}/Modules/_typesmodule.c
              ${PARSER_SRCS}
              ${OBJECT_SRCS}
              ${PYTHON_SRCS}
              ${MODULE_SRCS}
            #  ${SIGNAL_SRCS}
              )


target_link_libraries(${python_LIB_NAME} ${extraLibs})
#set_target_properties(pythonLib PROPERTIES OUTPUT_NAME ${python_LIB_NAME})

#add_executable(python Modules/python.c )
#target_link_libraries(python pythonLib)

#install(TARGETS pythonLib DESTINATION lib)
#install(TARGETS python DESTINATION bin)
#file(GLOB pyHeaders ${Python_SOURCE_DIR}/Include/*.h )
#install(FILES ${Python_BINARY_DIR}/pyconfig.h ${pyHeaders} DESTINATION include/python${PY_MAJOR}.${PY_MINOR})
