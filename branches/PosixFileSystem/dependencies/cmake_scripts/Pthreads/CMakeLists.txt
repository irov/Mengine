
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

project(pthreads)

SET(TRUE_DIR ../../pthreads)
SET(pthreads_SOURCE_DIR ${TRUE_DIR})

INCLUDE( ../menge_macro.cmake )

MENGE_SET_PROJECT_DEFAULTS( pthreads )

# OUR SOURCES
set(pthreads_sources
	${TRUE_DIR}/config.h
	${TRUE_DIR}/implement.h
	${TRUE_DIR}/semaphore.h
	${TRUE_DIR}/pthread.h
	${TRUE_DIR}/need_errno.h

	${TRUE_DIR}/attr.c
	${TRUE_DIR}/barrier.c
	${TRUE_DIR}/cancel.c
	${TRUE_DIR}/condvar.c
	${TRUE_DIR}/create.c
	${TRUE_DIR}/cleanup.c
	${TRUE_DIR}/global.c
	${TRUE_DIR}/exit.c
	${TRUE_DIR}/misc.c
	${TRUE_DIR}/mutex.c
	${TRUE_DIR}/nonportable.c
	${TRUE_DIR}/private.c
	${TRUE_DIR}/rwlock.c
	${TRUE_DIR}/sched.c
	${TRUE_DIR}/semaphore.c
	${TRUE_DIR}/spin.c
	${TRUE_DIR}/sync.c
	${TRUE_DIR}/tsd.c

)

add_definitions(-D_WIN32_WINNT=0x400 -DPTW32_STATIC_LIB -DPTW32_BUILD_INLINED -DHAVE_CONFIG_H -D__CLEANUP_C )

include_directories(${pthreads_SOURCE_DIR})
add_library( ${pthreads_LIB_NAME} STATIC ${pthreads_sources} )