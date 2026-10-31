#include "EmscriptenThreadSystem.h"

#include "Kernel/Assertion.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/ThreadMutexDummy.h"
#include "Kernel/ThreadSharedMutexDummy.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ThreadSystem, Mengine::EmscriptenThreadSystem );
//////////////////////////////////////////////////////////////////////////

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    EmscriptenThreadSystem::EmscriptenThreadSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EmscriptenThreadSystem::~EmscriptenThreadSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenThreadSystem::_initializeService()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void EmscriptenThreadSystem::_finalizeService()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenThreadSystem::isThreadSupported() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadIdentityInterfacePtr EmscriptenThreadSystem::createThreadIdentity( const ThreadDescription & _description, EThreadPriority _priority, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _description );
        MENGINE_UNUSED( _priority );
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_FATAL( false, "threads are not supported" );

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadProcessorInterfacePtr EmscriptenThreadSystem::createThreadProcessor( const ThreadDescription & _description, EThreadPriority _priority, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _description );
        MENGINE_UNUSED( _priority );
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_FATAL( false, "threads are not supported" );

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadMutexInterfacePtr EmscriptenThreadSystem::createMutex( const DocumentInterfacePtr & _doc )
    {
        ThreadMutexInterfacePtr mutex = Helper::makeFactorableUnique<ThreadMutexDummy>( _doc );

        return mutex;
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadSharedMutexInterfacePtr EmscriptenThreadSystem::createSharedMutex( const DocumentInterfacePtr & _doc )
    {
        ThreadSharedMutexInterfacePtr mutex = Helper::makeFactorableUnique<ThreadSharedMutexDummy>( _doc );

        return mutex;
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadConditionVariableInterfacePtr EmscriptenThreadSystem::createConditionVariable( const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_FATAL( false, "condition variables are not supported" );

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void EmscriptenThreadSystem::sleep( uint64_t _ms )
    {
        MENGINE_UNUSED( _ms );

        MENGINE_ASSERTION_FATAL( false, "sleep is not supported" );
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadId EmscriptenThreadSystem::getCurrentThreadId() const
    {
        return 1;
    }
    //////////////////////////////////////////////////////////////////////////
}
