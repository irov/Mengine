
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

project(pthreads)

SET( SOURCE_DIR ${CMAKE_SOURCE_DIR}/pthreads )

# OUR SOURCES
set(pthreads_sources
	${SOURCE_DIR}/config.h
	${SOURCE_DIR}/implement.h
	${SOURCE_DIR}/semaphore.h
	${SOURCE_DIR}/pthread.h
	${SOURCE_DIR}/need_errno.h

	${SOURCE_DIR}/attr.c
	${SOURCE_DIR}/barrier.c
	${SOURCE_DIR}/cancel.c
	${SOURCE_DIR}/condvar.c
	${SOURCE_DIR}/create.c
	${SOURCE_DIR}/cleanup.c
	${SOURCE_DIR}/global.c
	${SOURCE_DIR}/exit.c
	${SOURCE_DIR}/misc.c
	${SOURCE_DIR}/mutex.c
	${SOURCE_DIR}/nonportable.c
	${SOURCE_DIR}/private.c
	${SOURCE_DIR}/rwlock.c
	${SOURCE_DIR}/sched.c
	${SOURCE_DIR}/semaphore.c
	${SOURCE_DIR}/spin.c
	${SOURCE_DIR}/sync.c
	${SOURCE_DIR}/tsd.c

)

add_definitions(-D_WIN32_WINNT=0x400 -DPTW32_STATIC_LIB -DPTW32_BUILD_INLINED -DHAVE_CONFIG_H -D__CLEANUP_C )

include_directories(${SOURCE_DIR})
add_library( ${PROJECT_NAME}_static STATIC ${pthreads_sources} )