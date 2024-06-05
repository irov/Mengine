#include "RPAllocatorSystem.h"

#include "Interface/LoggerServiceInterface.h"

#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Error.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/LoggerHelper.h"

#include "Config/StdString.h"

#define ENABLE_PRELOAD 1

extern "C" {
#include "rpmalloc/rpmalloc.h"
}

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AllocatorSystem, Mengine::RPAllocatorSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    ////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        ////////////////////////////////////////////////////////////////////////
        static void rpmalloc_error_callback( const char * _message )
        {
            if( SERVICE_IS_INITIALIZE( LoggerServiceInterface ) == false )
            {
                return;
            }

            LOGGER_VERBOSE_LEVEL( Mengine::ConstString::none(), LM_ERROR, LFILTER_NONE, LCOLOR_RED, nullptr, 0 )("[rpmalloc] %s"
                , _message
                );
        }
        ////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    RPAllocatorSystem::RPAllocatorSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    RPAllocatorSystem::~RPAllocatorSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool RPAllocatorSystem::_initializeService()
    {
        rpmalloc_config_t config = {0};
        config.error_callback = &Detail::rpmalloc_error_callback;

        if( rpmalloc_initialize_config( &config ) != 0 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RPAllocatorSystem::_finalizeService()
    {
        rpmalloc_finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void * RPAllocatorSystem::malloc( size_t _size, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

        void * mem = rpmalloc( _size );

        MENGINE_ASSERTION_MEMORY_PANIC( mem, "invalid alloc memory '%zu' [%s]"
            , _size
            , this->getMemoryUsage()
            , _doc
        );

        return mem;
    }
    //////////////////////////////////////////////////////////////////////////
    void RPAllocatorSystem::free( void * _mem, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

        rpfree( _mem );
    }
    //////////////////////////////////////////////////////////////////////////
    void * RPAllocatorSystem::calloc( size_t _num, size_t _size, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

        void * mem = rpcalloc( _num, _size );

        MENGINE_ASSERTION_MEMORY_PANIC( mem, "invalid calloc memory '%zu' [%s]"
            , _num * _size
            , _doc
        );

        return mem;
    }
    //////////////////////////////////////////////////////////////////////////
    void * RPAllocatorSystem::realloc( void * _mem, size_t _size, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

        void * mem = rprealloc( _mem, _size );

        MENGINE_ASSERTION_MEMORY_PANIC( mem, "invalid realloc memory '%zu' from [%p] [%s]"
            , _size
            , _mem
            , _doc
        );

        return mem;
    }
    //////////////////////////////////////////////////////////////////////////
    void RPAllocatorSystem::startThread()
    {
        rpmalloc_thread_initialize();
    }
    //////////////////////////////////////////////////////////////////////////
    void RPAllocatorSystem::stopThread()
    {
        rpmalloc_thread_finalize( 1 );
    }
    //////////////////////////////////////////////////////////////////////////
}
