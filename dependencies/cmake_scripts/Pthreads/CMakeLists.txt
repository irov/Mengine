
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

project(pthreads)

SET(TRUE_DIR ../../pthreads)
SET(pthreads_SOURCE_DIR ${TRUE_DIR})

INCLUDE( ../menge_macro.cmake )

MENGE_SET_PROJECT_DEFAULTS()

# SET LIBNAME
# msvc does not append 'lib' - do it here to have consistent name
if(MSVC)
 set(pthreads_LIB_NAME lib)
endif(MSVC)
set( pthreads_LIB_NAME ${pthreads_LIB_NAME}pthreads_static )

# to distinguish between debug and release lib
set(CMAKE_DEBUG_POSTFIX "d")

# OUR SOURCES
set(libpthreads_sources
	${TRUE_DIR}/config.h
	${TRUE_DIR}/implement.h
	${TRUE_DIR}/semaphore.h
	${TRUE_DIR}/pthread.h
	${TRUE_DIR}/need_errno.h

	${TRUE_DIR}/attr.c
	${TRUE_DIR}/pthread_attr_init.c
	${TRUE_DIR}/pthread_attr_destroy.c
	${TRUE_DIR}/pthread_attr_getdetachstate.c
	${TRUE_DIR}/pthread_attr_setdetachstate.c
	${TRUE_DIR}/pthread_attr_getstackaddr.c
	${TRUE_DIR}/pthread_attr_setstackaddr.c
	${TRUE_DIR}/pthread_attr_getstacksize.c
	${TRUE_DIR}/pthread_attr_setstacksize.c
	${TRUE_DIR}/pthread_attr_getscope.c
	${TRUE_DIR}/pthread_attr_setscope.c

	${TRUE_DIR}/barrier.c
	${TRUE_DIR}/pthread_barrier_init.c
	${TRUE_DIR}/pthread_barrier_destroy.c
	${TRUE_DIR}/pthread_barrier_wait.c
	${TRUE_DIR}/pthread_barrierattr_init.c
	${TRUE_DIR}/pthread_barrierattr_destroy.c
	${TRUE_DIR}/pthread_barrierattr_setpshared.c
	${TRUE_DIR}/pthread_barrierattr_getpshared.c

	${TRUE_DIR}/cancel.c
	${TRUE_DIR}/pthread_setcancelstate.c
	${TRUE_DIR}/pthread_setcanceltype.c
	${TRUE_DIR}/pthread_testcancel.c
	${TRUE_DIR}/pthread_cancel.c 

	${TRUE_DIR}/condvar.c
	${TRUE_DIR}/ptw32_cond_check_need_init.c
	${TRUE_DIR}/pthread_condattr_destroy.c
	${TRUE_DIR}/pthread_condattr_getpshared.c
	${TRUE_DIR}/pthread_condattr_init.c
	${TRUE_DIR}/pthread_condattr_setpshared.c
	${TRUE_DIR}/pthread_cond_destroy.c
	${TRUE_DIR}/pthread_cond_init.c
	${TRUE_DIR}/pthread_cond_signal.c
	${TRUE_DIR}/pthread_cond_wait.c

	${TRUE_DIR}/create.c
	${TRUE_DIR}/cleanup.c
	${TRUE_DIR}/global.c

	${TRUE_DIR}/exit.c
	${TRUE_DIR}/pthread_exit.c

	${TRUE_DIR}/misc.c
	${TRUE_DIR}/pthread_equal.c
	${TRUE_DIR}/pthread_getconcurrency.c
	${TRUE_DIR}/pthread_kill.c
	${TRUE_DIR}/pthread_once.c
	${TRUE_DIR}/pthread_self.c
	${TRUE_DIR}/pthread_setconcurrency.c
	${TRUE_DIR}/ptw32_calloc.c
	${TRUE_DIR}/ptw32_MCS_lock.c
	${TRUE_DIR}/ptw32_new.c
	${TRUE_DIR}/ptw32_reuse.c
	${TRUE_DIR}/ptw32_relmillisecs.c
	${TRUE_DIR}/w32_CancelableWait.c

	${TRUE_DIR}/mutex.c
	${TRUE_DIR}/ptw32_mutex_check_need_init.c
	${TRUE_DIR}/pthread_mutex_init.c
	${TRUE_DIR}/pthread_mutex_destroy.c
	${TRUE_DIR}/pthread_mutexattr_init.c
	${TRUE_DIR}/pthread_mutexattr_destroy.c
	${TRUE_DIR}/pthread_mutexattr_getpshared.c
	${TRUE_DIR}/pthread_mutexattr_setpshared.c
	${TRUE_DIR}/pthread_mutexattr_settype.c
	${TRUE_DIR}/pthread_mutexattr_gettype.c
	${TRUE_DIR}/pthread_mutex_lock.c
	${TRUE_DIR}/pthread_mutex_timedlock.c
	${TRUE_DIR}/pthread_mutex_unlock.c
	${TRUE_DIR}/pthread_mutex_trylock.c

	${TRUE_DIR}/nonportable.c
	${TRUE_DIR}/pthread_mutexattr_setkind_np.c
	${TRUE_DIR}/pthread_mutexattr_getkind_np.c
	${TRUE_DIR}/pthread_getw32threadhandle_np.c
	${TRUE_DIR}/pthread_delay_np.c
	${TRUE_DIR}/pthread_num_processors_np.c
	${TRUE_DIR}/pthread_win32_attach_detach_np.c
	${TRUE_DIR}/pthread_timechange_handler_np.c

	${TRUE_DIR}/private.c
	${TRUE_DIR}/ptw32_is_attr.c
	${TRUE_DIR}/ptw32_processInitialize.c
	${TRUE_DIR}/ptw32_processTerminate.c
	${TRUE_DIR}/ptw32_threadStart.c
	${TRUE_DIR}/ptw32_threadDestroy.c
	${TRUE_DIR}/ptw32_tkAssocCreate.c
	${TRUE_DIR}/ptw32_tkAssocDestroy.c
	${TRUE_DIR}/ptw32_callUserDestroyRoutines.c
	${TRUE_DIR}/ptw32_semwait.c
	${TRUE_DIR}/ptw32_timespec.c
	${TRUE_DIR}/ptw32_throw.c
	${TRUE_DIR}/ptw32_InterlockedCompareExchange.c
	${TRUE_DIR}/ptw32_getprocessors.c

	${TRUE_DIR}/rwlock.c
	${TRUE_DIR}/ptw32_rwlock_check_need_init.c
	${TRUE_DIR}/ptw32_rwlock_cancelwrwait.c
	${TRUE_DIR}/pthread_rwlock_init.c
	${TRUE_DIR}/pthread_rwlock_destroy.c
	${TRUE_DIR}/pthread_rwlockattr_init.c
	${TRUE_DIR}/pthread_rwlockattr_destroy.c
	${TRUE_DIR}/pthread_rwlockattr_getpshared.c
	${TRUE_DIR}/pthread_rwlockattr_setpshared.c
	${TRUE_DIR}/pthread_rwlock_rdlock.c
	${TRUE_DIR}/pthread_rwlock_timedrdlock.c
	${TRUE_DIR}/pthread_rwlock_wrlock.c
	${TRUE_DIR}/pthread_rwlock_timedwrlock.c
	${TRUE_DIR}/pthread_rwlock_unlock.c
	${TRUE_DIR}/pthread_rwlock_tryrdlock.c
	${TRUE_DIR}/pthread_rwlock_trywrlock.c

	${TRUE_DIR}/sched.c
	${TRUE_DIR}/pthread_attr_setschedpolicy.c
	${TRUE_DIR}/pthread_attr_getschedpolicy.c
	${TRUE_DIR}/pthread_attr_setschedparam.c
	${TRUE_DIR}/pthread_attr_getschedparam.c
	${TRUE_DIR}/pthread_attr_setinheritsched.c
	${TRUE_DIR}/pthread_attr_getinheritsched.c
	${TRUE_DIR}/pthread_setschedparam.c
	${TRUE_DIR}/pthread_getschedparam.c
	${TRUE_DIR}/sched_get_priority_max.c
	${TRUE_DIR}/sched_get_priority_min.c
	${TRUE_DIR}/sched_setscheduler.c
	${TRUE_DIR}/sched_getscheduler.c
	${TRUE_DIR}/sched_yield.c

	${TRUE_DIR}/semaphore.c
	${TRUE_DIR}/sem_init.c
	${TRUE_DIR}/sem_destroy.c
	${TRUE_DIR}/sem_trywait.c
	${TRUE_DIR}/sem_timedwait.c
	${TRUE_DIR}/sem_wait.c
	${TRUE_DIR}/sem_post.c
	${TRUE_DIR}/sem_post_multiple.c
	${TRUE_DIR}/sem_getvalue.c
	${TRUE_DIR}/sem_open.c
	${TRUE_DIR}/sem_close.c
	${TRUE_DIR}/sem_unlink.c

	${TRUE_DIR}/spin.c
	${TRUE_DIR}/ptw32_spinlock_check_need_init.c
	${TRUE_DIR}/pthread_spin_init.c
	${TRUE_DIR}/pthread_spin_destroy.c
	${TRUE_DIR}/pthread_spin_lock.c
	${TRUE_DIR}/pthread_spin_unlock.c
	${TRUE_DIR}/pthread_spin_trylock.c

	${TRUE_DIR}/sync.c
	${TRUE_DIR}/pthread_detach.c
	${TRUE_DIR}/pthread_join.c

	${TRUE_DIR}/tsd.c
	${TRUE_DIR}/pthread_key_create.c
	${TRUE_DIR}/pthread_key_delete.c
	${TRUE_DIR}/pthread_setspecific.c
	${TRUE_DIR}/pthread_getspecific.c

)

add_definitions(-DPTW32_STATIC_LIB -DPTW32_BUILD_INLINED -DHAVE_CONFIG_H -DWall)

include_directories(${pthreads_SOURCE_DIR})
add_library( ${pthreads_LIB_NAME} STATIC ${libpthreads_sources} )