#include "AndroidKernelService.h"

#include "Interface/StringizeServiceInterface.h"

#include "Environment/Android/AndroidEnv.h"

#include "Kernel/ThreadMutexHelper.h"
#include "Kernel/ThreadMutexScope.h"
#include "Kernel/AssertionMemoryPanic.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AndroidKernelService, Mengine::AndroidKernelService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidKernelService::AndroidKernelService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidKernelService::~AndroidKernelService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidKernelService::_initializeService()
    {
        ThreadMutexInterfacePtr mutexJStrings = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );

        m_mutexJStrings = mutexJStrings;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidKernelService::_finalizeService()
    {
        m_mutexJStrings = nullptr;

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        for( IntrusiveListConstStringHolderJString::iterator
             it = m_holdersJString.begin();
             it != m_holdersJString.end(); )
        {
            IntrusiveListConstStringHolderJString::iterator it_erase = it;

            ConstStringHolderJString * holder = *it;
            ++it;

            m_holdersJString.erase( it_erase );

            holder->removeJString( jenv );

            m_poolJString.destroyT( holder );
        }

        m_holdersJString.clear();
        m_poolJString.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidKernelService::stringize( JNIEnv * _jenv, jstring _value, ConstString * const _cstr )
    {
        jsize value_length = _jenv->GetStringLength( _value );

        if( value_length == 0 )
        {
            *_cstr = ConstString::none();

            return;
        }

        MENGINE_THREAD_MUTEX_SCOPE( m_mutexJStrings );

        ConstStringHolderJString * holder = m_poolJString.createT();

        holder->setJString( _jenv, _value );

        if( STRINGIZE_SERVICE()
            ->stringizeExternal( holder, _cstr ) == false )
        {
            holder->removeJString( _jenv );

            m_poolJString.destroyT( holder );

            return;
        }

        m_holdersJString.push_back( holder );
    }
    //////////////////////////////////////////////////////////////////////////
}