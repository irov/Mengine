cmake_minimum_required(VERSION 3.2)

project(python3)

SET(SOURCE_DIR ${THIRDPARTY_DIR}/python3)

configure_file(Modified/config.c ${SOURCE_DIR}/PC/config.c COPYONLY)
configure_file(Modified/pyconfig.h ${SOURCE_DIR}/PC/pyconfig.h COPYONLY)

ADD_DEFINITIONS(-D_Py_HAVE_ZLIB)

REMOVE_DEFINITIONS(-Doverride)

#option(WITH_THREAD "compile in rudimentary thread support" OFF)
#option(WITH_DOC_STRINGS "Enable if you want documentation strings in extension modules" OFF)
#option(Py_USING_UNICODE "Enable unicode support" ON)
#option(Py_DEBUG "Build debugging interpreter" OFF)
#option(WITH_PYMALLOC "Define if you want to compile in Python-specific mallocs" ON)
#option(HAVE_LARGEFILE_SUPPORT "Large file support " ON)

set(extraLibs)

if(HAVE_LIBM)
  set(extraLibs ${extraLibs} ${HAVE_LIBM})
endif()

if(HAVE_LIBUTIL)
  set(extraLibs ${extraLibs} ${HAVE_LIBUTIL})
endif()

set(MOD_SRCS)

# this macro let's you decide whether you want to build the macro 
# and if yes, whether you want to build it statically or as module
macro(ADD_PYTHON_MODULE _NAME _STATIC_SRCS)
  option(MODULE_${_NAME}_ENABLE "Add module ${_NAME}" TRUE)

  if(MODULE_${_NAME}_ENABLE)
    option(MODULE_${_NAME}_SHARED "Add module ${_NAME} shared" FALSE)
    if(MODULE_${_NAME}_SHARED)
      message(STATUS "NOT Adding ${_NAME} as shared module (not yet implemented)")
    else()
      message(STATUS "Adding ${_NAME} as static module")
      foreach (_current_FILE ${ARGN})
        set(${_STATIC_SRCS} ${${_STATIC_SRCS}} ${_current_FILE})
      endforeach (_current_FILE ${ARGN})
      set(PY_STATIC_MODULES_INIT_FUNCTIONS "${PY_STATIC_MODULES_INIT_FUNCTIONS}extern void init${_NAME}(void);\n")
      set(PY_STATIC_MODULES_INITTAB        "${PY_STATIC_MODULES_INITTAB}  {\"${_NAME}\", init${_NAME}},\n")
    endif()
  endif()
endmacro()

if(WIN32)
  set(PY_POSIX_NAME nt)
else()
  set(PY_POSIX_NAME posix)
endif()

#add_python_module(${PY_POSIX_NAME}  MOD_SRCS ${SOURCE_DIR}/Modules/posixmodule.c)

add_python_module(signal    MOD_SRCS ${SOURCE_DIR}/Modules/signalmodule.c)
add_python_module(errno     MOD_SRCS ${SOURCE_DIR}/Modules/errnomodule.c)
#add_python_module(pwd       MOD_SRCS ${TRUE_DIR}/Modules/pwdmodule.c)
add_python_module(_sre      MOD_SRCS ${SOURCE_DIR}/Modules/_sre/sre.c)
add_python_module(_codecs   MOD_SRCS ${SOURCE_DIR}/Modules/_codecsmodule.c)
add_python_module(_symtable MOD_SRCS ${SOURCE_DIR}/Modules/symtablemodule.c)
add_python_module(xxsubtype MOD_SRCS ${SOURCE_DIR}/Modules/xxsubtype.c)
add_python_module(_struct MOD_SRCS ${SOURCE_DIR}/Modules/_struct.c)
add_python_module(binascii MOD_SRCS ${SOURCE_DIR}/Modules/binascii.c)
add_python_module(_statistics MOD_SRCS ${SOURCE_DIR}/Modules/_statisticsmodule.c)
add_python_module(_xxsubinterpreters MOD_SRCS ${SOURCE_DIR}/Modules/_xxsubinterpretersmodule.c)



set( HAVE_CHROOT 0 )
set( HAVE_FSYNC 0 )
set( HAVE_WAIT3 0 )
set( HAVE_WAIT4 0 )
set( HAVE_DEVICE_MACROS 0 )


############################## 

set(PARSER_SRCS
        ${SOURCE_DIR}/Parser/action_helpers.c
        ${SOURCE_DIR}/Parser/myreadline.c
        ${SOURCE_DIR}/Parser/parser.c
        ${SOURCE_DIR}/Parser/peg_api.c
        ${SOURCE_DIR}/Parser/pegen.c
        ${SOURCE_DIR}/Parser/pegen_errors.c
        ${SOURCE_DIR}/Parser/string_parser.c
        ${SOURCE_DIR}/Parser/token.c
        ${SOURCE_DIR}/Parser/tokenizer.c
        )


set(OBJECT_SRCS
        ${SOURCE_DIR}/Objects/abstract.c
        ${SOURCE_DIR}/Objects/call.c
        ${SOURCE_DIR}/Objects/boolobject.c
        ${SOURCE_DIR}/Objects/bytes_methods.c
        ${SOURCE_DIR}/Objects/bytearrayobject.c
        ${SOURCE_DIR}/Objects/bytesobject.c
        ${SOURCE_DIR}/Objects/capsule.c
		${SOURCE_DIR}/Objects/cellobject.c
        ${SOURCE_DIR}/Objects/classobject.c
        ${SOURCE_DIR}/Objects/codeobject.c
        ${SOURCE_DIR}/Objects/complexobject.c
        ${SOURCE_DIR}/Objects/stringlib/count.h
        ${SOURCE_DIR}/Objects/descrobject.c
        ${SOURCE_DIR}/Objects/dictobject.c
        ${SOURCE_DIR}/Objects/enumobject.c
        ${SOURCE_DIR}/Objects/exceptions.c
        ${SOURCE_DIR}/Objects/stringlib/fastsearch.h
        ${SOURCE_DIR}/Objects/fileobject.c
        ${SOURCE_DIR}/Objects/stringlib/find.h
        ${SOURCE_DIR}/Objects/floatobject.c
        ${SOURCE_DIR}/Objects/frameobject.c
        ${SOURCE_DIR}/Objects/funcobject.c
        ${SOURCE_DIR}/Objects/genobject.c
        ${SOURCE_DIR}/Objects/iterobject.c
        ${SOURCE_DIR}/Objects/listobject.c
        ${SOURCE_DIR}/Objects/longobject.c
        ${SOURCE_DIR}/Objects/memoryobject.c
        ${SOURCE_DIR}/Objects/methodobject.c
        ${SOURCE_DIR}/Objects/moduleobject.c
        ${SOURCE_DIR}/Objects/object.c
        ${SOURCE_DIR}/Objects/obmalloc.c
        ${SOURCE_DIR}/Objects/stringlib/partition.h
        ${SOURCE_DIR}/Objects/rangeobject.c
        ${SOURCE_DIR}/Objects/setobject.c
        ${SOURCE_DIR}/Objects/sliceobject.c
        ${SOURCE_DIR}/Objects/stringlib/split.h
        ${SOURCE_DIR}/Objects/structseq.c
        ${SOURCE_DIR}/Objects/tupleobject.c
        ${SOURCE_DIR}/Objects/typeobject.c
        ${SOURCE_DIR}/Objects/unicodectype.c
        ${SOURCE_DIR}/Objects/unicodeobject.c
        ${SOURCE_DIR}/Objects/unicodetype_db.h
        ${SOURCE_DIR}/Objects/weakrefobject.c
        ${SOURCE_DIR}/Objects/namespaceobject.c
        ${SOURCE_DIR}/Objects/odictobject.c
        ${SOURCE_DIR}/Objects/picklebufobject.c
        ${SOURCE_DIR}/Objects/interpreteridobject.c
        )
   
set(OBJECT_SRCS ${OBJECT_SRCS} ${SOURCE_DIR}/Objects/unicodeobject.c ${SOURCE_DIR}/Objects/unicodectype.c)


set(PYTHON_SRCS
        ${SOURCE_DIR}/Python/_warnings.c
        ${SOURCE_DIR}/Python/preconfig.c
        ${SOURCE_DIR}/Python/initconfig.c
        ${SOURCE_DIR}/Python/asdl.c
        ${SOURCE_DIR}/Python/ast.c
        ${SOURCE_DIR}/Python/ast_opt.c
        ${SOURCE_DIR}/Python/ast_unparse.c
        ${SOURCE_DIR}/Python/bltinmodule.c
        ${SOURCE_DIR}/Python/ceval.c
        ${SOURCE_DIR}/Python/codecs.c
        ${SOURCE_DIR}/Python/compile.c
        ${SOURCE_DIR}/Python/dynamic_annotations.c
        ${SOURCE_DIR}/Python/errors.c
        ${SOURCE_DIR}/Python/fileutils.c
        ${SOURCE_DIR}/Python/formatter_unicode.c
        ${SOURCE_DIR}/Python/frozen.c
        ${SOURCE_DIR}/Python/future.c
        ${SOURCE_DIR}/Python/getargs.c
        ${SOURCE_DIR}/Python/getcompiler.c
        ${SOURCE_DIR}/Python/getcopyright.c
        ${SOURCE_DIR}/Python/getopt.c
        ${SOURCE_DIR}/Python/getplatform.c
        ${SOURCE_DIR}/Python/getversion.c
        ${SOURCE_DIR}/Python/import.c
        ${SOURCE_DIR}/Python/importdl.c
        ${SOURCE_DIR}/Python/importdl.h
        ${SOURCE_DIR}/Python/marshal.c
        ${SOURCE_DIR}/Python/modsupport.c
        ${SOURCE_DIR}/Python/mysnprintf.c
        ${SOURCE_DIR}/Python/mystrtoul.c
        ${SOURCE_DIR}/Python/pyarena.c
        ${SOURCE_DIR}/Python/pyctype.c
        ${SOURCE_DIR}/Python/pyfpe.c
        ${SOURCE_DIR}/Python/pymath.c
        ${SOURCE_DIR}/Python/pytime.c
        ${SOURCE_DIR}/Python/pystate.c
        ${SOURCE_DIR}/Python/pystrcmp.c
        ${SOURCE_DIR}/Python/pystrtod.c
        ${SOURCE_DIR}/Python/dtoa.c
        ${SOURCE_DIR}/Python/Python-ast.c
        ${SOURCE_DIR}/Python/pythonrun.c
        ${SOURCE_DIR}/Python/structmember.c
        ${SOURCE_DIR}/Python/symtable.c
        ${SOURCE_DIR}/Python/sysmodule.c
        ${SOURCE_DIR}/Python/thread.c
        ${SOURCE_DIR}/Python/thread_nt.h
        ${SOURCE_DIR}/Python/traceback.c
        ${SOURCE_DIR}/Python/dynload_win.c
        ${SOURCE_DIR}/Python/pyhash.c    
        ${SOURCE_DIR}/Python/pylifecycle.c
        ${SOURCE_DIR}/Python/pystrhex.c
        ${SOURCE_DIR}/Python/bootstrap_hash.c
        ${SOURCE_DIR}/Python/pyhash.c
        ${SOURCE_DIR}/Python/pathconfig.c
        ${SOURCE_DIR}/Python/context.c
        ${SOURCE_DIR}/Python/hamt.c
        
        ${SOURCE_DIR}/PC/config.c
        ${SOURCE_DIR}/PC/dl_nt.c
        ${SOURCE_DIR}/PC/msvcrtmodule.c
        ${SOURCE_DIR}/PC/winreg.c
        ${SOURCE_DIR}/PC/invalid_parameter_handler.c
        
        
        ${DYNLOAD_SRC}
        )

set(MODULE_SRCS
        ${SOURCE_DIR}/Modules/_bisectmodule.c
        ${SOURCE_DIR}/Modules/_codecsmodule.c
        ${SOURCE_DIR}/Modules/_collectionsmodule.c
        ${SOURCE_DIR}/Modules/_csv.c
        ${SOURCE_DIR}/Modules/_functoolsmodule.c
        ${SOURCE_DIR}/Modules/_heapqmodule.c
        ${SOURCE_DIR}/Modules/_json.c
        ${SOURCE_DIR}/Modules/_localemodule.c
        ${SOURCE_DIR}/Modules/_lsprof.c
        ${SOURCE_DIR}/Modules/_math.h
        ${SOURCE_DIR}/Modules/_pickle.c
        ${SOURCE_DIR}/Modules/_randommodule.c
        ${SOURCE_DIR}/Modules/_struct.c
        ${SOURCE_DIR}/Modules/_weakref.c
        ${SOURCE_DIR}/Modules/_tracemalloc.c
        ${SOURCE_DIR}/Modules/_opcode.c
        ${SOURCE_DIR}/Modules/_stat.c
        ${SOURCE_DIR}/Modules/_winapi.c
        ${SOURCE_DIR}/Modules/_operator.c
        ${SOURCE_DIR}/Modules/_contextvarsmodule.c
        ${SOURCE_DIR}/Modules/_ssl.c
        ${SOURCE_DIR}/Modules/_abc.c
        
        ${SOURCE_DIR}/Modules/_multiprocessing/multiprocessing.c
        ${SOURCE_DIR}/Modules/_multiprocessing/posixshmem.c
        ${SOURCE_DIR}/Modules/_multiprocessing/semaphore.c

        ${SOURCE_DIR}/Modules/cjkcodecs/_codecs_cn.c
        ${SOURCE_DIR}/Modules/cjkcodecs/_codecs_hk.c
        ${SOURCE_DIR}/Modules/cjkcodecs/_codecs_iso2022.c
        ${SOURCE_DIR}/Modules/cjkcodecs/_codecs_jp.c
        ${SOURCE_DIR}/Modules/cjkcodecs/_codecs_kr.c
        ${SOURCE_DIR}/Modules/cjkcodecs/_codecs_tw.c
                        
        ${SOURCE_DIR}/Modules/_io/_iomodule.c
        ${SOURCE_DIR}/Modules/_io/bufferedio.c
        ${SOURCE_DIR}/Modules/_io/bytesio.c
        ${SOURCE_DIR}/Modules/_io/fileio.c
        ${SOURCE_DIR}/Modules/_io/iobase.c
        ${SOURCE_DIR}/Modules/_io/stringio.c
        ${SOURCE_DIR}/Modules/_io/textio.c
        ${SOURCE_DIR}/Modules/_io/winconsoleio.c
        
        ${SOURCE_DIR}/Modules/_blake2/blake2b_impl.c
        ${SOURCE_DIR}/Modules/_blake2/blake2module.c
        ${SOURCE_DIR}/Modules/_blake2/blake2s_impl.c
        
        ${SOURCE_DIR}/Modules/cjkcodecs/multibytecodec.c
        
        ${SOURCE_DIR}/Modules/arraymodule.c
        ${SOURCE_DIR}/Modules/atexitmodule.c
        ${SOURCE_DIR}/Modules/audioop.c
        ${SOURCE_DIR}/Modules/binascii.c
        ${SOURCE_DIR}/Modules/cmathmodule.c
        ${SOURCE_DIR}/Modules/errnomodule.c
        ${SOURCE_DIR}/Modules/gcmodule.c
        ${SOURCE_DIR}/Modules/itertoolsmodule.c
        ${SOURCE_DIR}/Modules/main.c
        ${SOURCE_DIR}/Modules/mathmodule.c
        ${SOURCE_DIR}/Modules/md5module.c
        ${SOURCE_DIR}/Modules/mmapmodule.c
        ${SOURCE_DIR}/Modules/posixmodule.c
        ${SOURCE_DIR}/Modules/rotatingtree.c
        ${SOURCE_DIR}/Modules/rotatingtree.h
        ${SOURCE_DIR}/Modules/sha1module.c
        ${SOURCE_DIR}/Modules/signalmodule.c
        ${SOURCE_DIR}/Modules/symtablemodule.c
        ${SOURCE_DIR}/Modules/_threadmodule.c
        ${SOURCE_DIR}/Modules/_datetimemodule.c
        ${SOURCE_DIR}/Modules/xxsubtype.c
        ${SOURCE_DIR}/Modules/unicodedata.c
        
        ${SOURCE_DIR}/Modules/faulthandler.c
        
        ${SOURCE_DIR}/Modules/pyexpat.c
        
        ${SOURCE_DIR}/Modules/socketmodule.c
      
        ${SOURCE_DIR}/Modules/expat/xmlparse.c
        ${SOURCE_DIR}/Modules/expat/xmlrole.c
        ${SOURCE_DIR}/Modules/expat/xmltok.c
        ${SOURCE_DIR}/Modules/expat/xmltok_impl.c
        ${SOURCE_DIR}/Modules/expat/xmltok_ns.c
        
        ${SOURCE_DIR}/Modules/zlibmodule.c
        
        
        ${SOURCE_DIR}/Modules/timemodule.c
        ${SOURCE_DIR}/Modules/selectmodule.c
        ${MOD_SRCS} )
        
if(NOT WIN32)
    set(MODULE_SRCS ${MODULE_SRCS} ${SOURCE_DIR}/Modules/getpath.c)
endif()

include_directories(${OPENSSL_INCLUDE_DIR})

set_source_files_properties(${SOURCE_DIR}/Python/getplatform.c PROPERTIES COMPILE_FLAGS -DPLATFORM=\\"${PY_PLATFORM}\\")
set_source_files_properties(${SOURCE_DIR}/Modules/getpath.c PROPERTIES COMPILE_FLAGS  "-DPREFIX=\\\"${CMAKE_INSTALL_PREFIX}\\\" -DEXEC_PREFIX=\\\"${CMAKE_INSTALL_PREFIX}\\\" -DVERSION=\\\"3.4.2\\\" -DVPATH=\\\"..\\\"  -DPYTHONPATH=\\\":plat-linux2:lib-tk\\\" ")

include_directories(${SOURCE_DIR})
include_directories(${SOURCE_DIR}/Include)
include_directories(${SOURCE_DIR}/Include/internal)

include_directories(${THIRDPARTY_CONFIG_DIR}/zlib)
include_directories(${THIRDPARTY_DIR}/zlib)

include_directories(${THIRDPARTY_CONFIG_DIR}/openssl)

include_directories(${SOURCE_DIR}/PC)
include_directories(${SOURCE_DIR}/Python)
include_directories(${SOURCE_DIR}/Modules/expat)

if(WIN32)
  add_definitions(-D_WINDOWS)
    
  if(MSVC)
    add_definitions(-DWIN32)
  endif()
endif()

if(MINGW)
  add_definitions(-D_WINDOWS)
  add_definitions(-DWIN64)
endif()

add_definitions(-DPYEXPAT_EXPORTS)
add_definitions(-DHAVE_EXPAT_H)
add_definitions(-DXML_NS)
add_definitions(-DXML_DTD)
add_definitions(-DBYTEORDER=1234)
add_definitions(-DXML_CONTEXT_BYTES=1024)
add_definitions(-DXML_STATIC)
add_definitions(-DHAVE_MEMMOVE)
add_definitions(-DPY3_DLLNAME="python3.dll")
add_definitions(-DMS_NO_COREDLL)
add_definitions(-DPy_NO_ENABLE_SHARED)
add_definitions(-DPy_BUILD_CORE_BUILTIN)

add_library(${PROJECT_NAME} STATIC
            ${SOURCE_DIR}/Modules/getbuildinfo.c 
            ${PARSER_SRCS}
            ${OBJECT_SRCS}
            ${PYTHON_SRCS}
            ${MODULE_SRCS}
            )

target_link_libraries(${PROJECT_NAME} ${extraLibs})