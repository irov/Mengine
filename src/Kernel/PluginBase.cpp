#include "PluginBase.h"

#include "Interface/ThreadServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static void s_stdex_thread_lock( ThreadMutexInterface * _mutex )
    {
        _mutex->lock();
    }
    //////////////////////////////////////////////////////////////////////////
    static void s_stdex_thread_unlock( ThreadMutexInterface * _mutex )
    {
        _mutex->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    PluginBase::PluginBase()
        : m_dynamicLoad( false )
        , m_initializePlugin( false )
        , m_availablePlugin( true )
        , m_systemPlugin( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PluginBase::~PluginBase()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void PluginBase::setDynamicLoad( bool _dynamicLoad )
    {
        m_dynamicLoad = _dynamicLoad;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PluginBase::isDynamicLoad() const
    {
        return m_dynamicLoad;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PluginBase::_availablePlugin() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PluginBase::_systemPlugin() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PluginBase::initializePlugin()
    {
        if( m_initializePlugin == true )
        {
            return true;
        }

        m_availablePlugin = this->_availablePlugin();

        if( m_availablePlugin == false )
        {
            LOGGER_WARNING( "plugin '%s' not available"
                , this->getPluginName()
            );

            return true;
        }

        if( m_dynamicLoad == true )
        {
            SERVICE_WAIT( ThreadServiceInterface, [this]()
            {
                m_mutexAllocatorPool = THREAD_SERVICE()
                    ->createMutex( MENGINE_DOCUMENT( "Plugin '%s'", this->getPluginName() ) );

                stdex_allocator_initialize_threadsafe( m_mutexAllocatorPool.get()
                    , (stdex_allocator_thread_lock_t)& s_stdex_thread_lock
                    , (stdex_allocator_thread_unlock_t)& s_stdex_thread_unlock
                );

                return true;
            } );
        }

        bool successful = this->_initializePlugin();

        if( successful == false )
        {
            LOGGER_WARNING( "plugin '%s' not initialize"
                , this->getPluginName()
            );

            return false;
        }

        m_initializePlugin = true;

        m_systemPlugin = this->_systemPlugin();

        NOTIFICATION_NOTIFY( NOTIFICATOR_PLUGIN_INITIALIZE, this->getPluginName() );

        return m_initializePlugin;
    }
    //////////////////////////////////////////////////////////////////////////
    void PluginBase::finalizePlugin()
    {
        if( m_initializePlugin == false )
        {
            return;
        }

        m_initializePlugin = false;

        NOTIFICATION_NOTIFY( NOTIFICATOR_PLUGIN_FINALIZE, this->getPluginName() );

        this->_finalizePlugin();

        for( const ConstString & name : m_moduleFactories )
        {
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Module" ), name );
        }

        m_moduleFactories.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool PluginBase::isInitializePlugin() const
    {
        return m_initializePlugin;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PluginBase::isAvailablePlugin() const
    {
        return m_availablePlugin;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PluginBase::isSystemPlugin() const
    {
        return m_systemPlugin;
    }
    //////////////////////////////////////////////////////////////////////////
    void PluginBase::destroy()
    {
        this->_destroyPlugin();
        this->_destroy();

        bool dynamicLoad = m_dynamicLoad;

        ThreadMutexInterfacePtr mutexAllocatorPool = std::move( m_mutexAllocatorPool );

        delete this;

        if( dynamicLoad == true )
        {
            stdex_allocator_finalize_threadsafe();
            mutexAllocatorPool = nullptr;

            stdex_allocator_finalize();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool PluginBase::_initializePlugin()
    {
        ///Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PluginBase::_finalizePlugin()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void PluginBase::_destroyPlugin()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool PluginBase::addModuleFactory( const ConstString & _name, const ModuleFactoryInterfacePtr & _factory )
    {
        VOCABULARY_SET( ModuleFactoryInterface, STRINGIZE_STRING_LOCAL( "Module" ), _name, _factory );
        
        m_moduleFactories.emplace_back( _name );

        return true;
    }
}