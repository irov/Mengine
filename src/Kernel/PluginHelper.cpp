#include "PluginBase.h"

#include "Interface/ThreadServiceInterface.h"
#include "Interface/FactoryServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionObservable.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/VocabularyHelper.h"

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
    const ServiceRequiredList & PluginBase::requiredServices() const
    {
        static ServiceRequiredList required = {};

        return required;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PluginBase::initializePlugin()
    {
        if( m_initializePlugin == true )
        {
            return true;
        }

        const ServiceRequiredList & required = this->requiredServices();

        for( const Char * serviceId : required )
        {
            const bool * serviceInitialized = SERVICE_PROVIDER_GET()
                ->isInitializeServiceProvider( serviceId );

            if( *serviceInitialized == true )
            {
                continue;
            }

            bool unimportantPlugin = this->_unimportantPlugin();

            if( unimportantPlugin == true )
            {
                LOGGER_MESSAGE( "plugin '%s' required service '%s' [unimportant]"
                    , this->getPluginName()
                    , serviceId
                );

                return true;
            }

            LOGGER_ERROR( "plugin '%s' required service '%s'"
                , this->getPluginName()
                , serviceId
            );

            return false;
        }

        m_availablePlugin = this->_availablePlugin();

        if( m_availablePlugin == false )
        {
            LOGGER_INFO( "plugin", "plugin not available '%s'"
                , this->getPluginName()
            );

            return true;
        }        

        bool successful = false;

        try
        {
            successful = this->_initializePlugin();
        }
        catch( const std::exception & ex )
        {
            LOGGER_ERROR( "plugin '%s' initialize exception: %s"
                , this->getPluginName()
                , ex.what()
            );
        }

        if( successful == false )
        {
            bool unimportantPlugin = this->_unimportantPlugin();

            if( unimportantPlugin == true )
            {
                LOGGER_MESSAGE( "plugin '%s' not initialize [unimportant]"
                    , this->getPluginName()
                );

                return true;
            }

            LOGGER_ERROR( "plugin '%s' not initialize"
                , this->getPluginName()
            );

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
    void PluginBase::_destroy()
    {
        const Char * pluginName = this->getPluginName();
        MENGINE_UNUSED( pluginName );

        MENGINE_ASSERTION_OBSERVABLE( this, "plugin '%s'"
            , this->getPluginName()
        );

        this->_destroyPlugin();

#if defined(MENGINE_DEBUG_FACTORY_ENABLE) && defined(MENGINE_DOCUMENT_ENABLE)
        bool dynamicLoad = m_dynamicLoad;

        if( dynamicLoad == true )
        {
            typedef Vector<DocumentInterfacePtr> VectorDocuments;
            VectorDocuments leakObjects;

            FACTORY_SERVICE()
                ->debugFactoryForeachLeakObjects( ~0U, [&leakObjects]( const FactoryInterface * _factory, const Factorable * _factorable )
            {
                MENGINE_UNUSED( _factory );
                MENGINE_UNUSED( _factorable );

                const DocumentInterfacePtr & doc = _factorable->getDocument();

                if( doc == nullptr )
                {
                    return;
                }

                const Char * object_modulePath = doc->getModulePath();

                if( StdString::strcmp( MENGINE_CODE_LIBRARY, object_modulePath ) != 0 )
                {
                    return;
                }

                leakObjects.emplace_back( doc );
            } );

            if( leakObjects.empty() == false )
            {
                LOGGER_MESSAGE_RELEASE( "plugin [%s] leak %zu objects"
                    , pluginName
                    , leakObjects.size()
                );

                for( const DocumentInterfacePtr & doc : leakObjects )
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
    bool PluginBase::addModuleFactory( const ConstString & _name, const ModuleFactoryInterfacePtr & _factory, const DocumentInterfacePtr & _doc )
    {
        VOCABULARY_SET( ModuleFactoryInterface, STRINGIZE_STRING_LOCAL( "Module" ), _name, _factory, _doc );

        m_moduleFactories.emplace_back( _name );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}