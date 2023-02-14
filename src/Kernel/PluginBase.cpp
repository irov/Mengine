#include "PluginBase.h"

#include "Interface/ThreadServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/FactoryServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionObservable.h"
#include "Kernel/NotificationHelper.h"

#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PluginBase::PluginBase()
        : m_uid( INVALID_UNIQUE_ID )
        , m_dynamicLoad( false )
        , m_initializePlugin( false )
        , m_availablePlugin( true )
        , m_systemPlugin( false )
        , m_unimportantPlugin( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PluginBase::~PluginBase()
    {
        //MENGINE_ASSERTION_OBSERVABLE -> move to ::_destroy
    }
    //////////////////////////////////////////////////////////////////////////
    void PluginBase::setUID( uint32_t _uid )
    {
        m_uid = _uid;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t PluginBase::getUID() const
    {
        return m_uid;
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
    bool PluginBase::_unimportantPlugin() const
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
            LOGGER_INFO( "plugin", "plugin not available '%s'"
                , this->getPluginName()
            );

            return true;
        }

        m_unimportantPlugin = this->_unimportantPlugin();

        bool successful = false;

        try
        {
            successful = this->_initializePlugin();
        }
        catch( const std::exception & ex )
        {
            LOGGER_ERROR( "plugin '%s' initialize exception:\n%s"
                , this->getPluginName()
                , ex.what()
            );
        }

        if( successful == false )
        {
            LOGGER_ERROR( "plugin '%s' not initialize"
                , this->getPluginName()
            );

            if( m_unimportantPlugin == true )
            {
                return true;
            }

            return false;
        }

        m_initializePlugin = true;

        m_systemPlugin = this->_systemPlugin();

        const Char * pluginName = this->getPluginName();

        NOTIFICATION_NOTIFY( NOTIFICATOR_PLUGIN_INITIALIZE, pluginName );

        LOGGER_INFO( "plugin", "plugin initialize '%s'"
            , this->getPluginName()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PluginBase::finalizePlugin()
    {
        const Char * pluginName = this->getPluginName();

        SERVICE_PROVIDER_GET()
            ->unlinkService( pluginName );

        if( m_initializePlugin == false )
        {
            return;
        }

        m_initializePlugin = false;

        NOTIFICATION_NOTIFY( NOTIFICATOR_PLUGIN_FINALIZE, pluginName );

        this->_finalizePlugin();

        for( const ConstString & name : m_moduleFactories )
        {
            ModuleFactoryInterfacePtr module = VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Module" ), name );

            module->finalize();
        }

        m_moduleFactories.clear();

        LOGGER_INFO( "plugin", "plugin finalize '%s'"
            , this->getPluginName()
        );
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
    bool PluginBase::isUnimportantPlugin() const
    {
        return m_unimportantPlugin;
    }
    //////////////////////////////////////////////////////////////////////////
    void PluginBase::_destroy()
    {
        const Char * pluginName = this->getPluginName();
        MENGINE_UNUSED( pluginName );

        MENGINE_ASSERTION_OBSERVABLE( this, "plugin '%s'"
            , this->getPluginName()
        );

        this->_destroyPlugin();

#if defined(MENGINE_DEBUG_FACTORY_ENABLE)
        bool dynamicLoad = m_dynamicLoad;

        if( dynamicLoad == true )
        {
            typedef Vector<DocumentPtr> VectorDocuments;
            VectorDocuments leakObjects;

            FACTORY_SERVICE()
                ->debugFactoryForeachLeakObjects( ~0U, [&leakObjects]( const FactoryInterface * _factory, const Factorable * _factorable )
            {
                MENGINE_UNUSED( _factory );
                MENGINE_UNUSED( _factorable );

                const DocumentPtr & doc = _factorable->getDocument();

                if( doc == nullptr )
                {
                    return;
                }

                const Char * object_modulePath = doc->getModulePath();

                if( MENGINE_STRCMP( MENGINE_LIBRARY_NAME, object_modulePath ) != 0 )
                {
                    return;
                }

                leakObjects.emplace_back( doc );
            } );

            if( leakObjects.empty() == false )
            {
                LOGGER_MESSAGE_RELEASE( "plugin [%s] leak %u objects"
                    , pluginName
                    , leakObjects.size()
                );

                for( const DocumentPtr & doc : leakObjects )
                {
                    LOGGER_MESSAGE_RELEASE( "-- %s"
                        , MENGINE_DOCUMENT_STR( doc )
                    );
                }
            }
        }
#endif
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
    bool PluginBase::addModuleFactory( const ConstString & _name, const ModuleFactoryInterfacePtr & _factory, const DocumentPtr & _doc )
    {
        VOCABULARY_SET( ModuleFactoryInterface, STRINGIZE_STRING_LOCAL( "Module" ), _name, _factory, _doc );

        m_moduleFactories.emplace_back( _name );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}