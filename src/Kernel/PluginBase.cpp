#include "PluginBase.h"

#include "Interface/ThreadServiceInterface.h"
#include "Interface/FactoryServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionObservable.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/VocabularyHelper.h"

#include "Config/StdString.h"
#include "Config/StdException.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PluginBase::PluginBase()
        : m_dynamicLoad( false )
        , m_frameworkMode( false )
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
    void PluginBase::setFrameworkMode( bool _frameworkMode )
    {
        m_frameworkMode = _frameworkMode;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PluginBase::isFrameworkMode() const
    {
        return m_frameworkMode;
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
    bool PluginBase::registerPlugin( const PluginInterfacePtr & _plugin )
    {
        MENGINE_UNUSED( _plugin );

        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PluginBase::unregisterPlugin( const PluginInterfacePtr & _plugin )
    {
        MENGINE_UNUSED( _plugin );

        //Empty
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
                    , this->getPluginName().c_str()
                    , serviceId
                );

                return true;
            }

            LOGGER_ERROR( "plugin '%s' required service '%s'"
                , this->getPluginName().c_str()
                , serviceId
            );

            return false;
        }

        m_availablePlugin = this->_availablePlugin();

        if( m_availablePlugin == false )
        {
            LOGGER_INFO( "plugin", "plugin not available '%s'"
                , this->getPluginName().c_str()
            );

            return true;
        }        

        bool successful = false;

        try
        {
            successful = this->_initializePlugin();
        }
        catch( const StdException::exception & ex )
        {
            LOGGER_ERROR( "plugin '%s' initialize exception: %s"
                , this->getPluginName().c_str()
                , ex.what()
            );
        }

        if( successful == false )
        {
            bool unimportantPlugin = this->_unimportantPlugin();

            if( unimportantPlugin == true )
            {
                LOGGER_MESSAGE( "plugin '%s' not initialize [unimportant]"
                    , this->getPluginName().c_str()
                );

                return true;
            }

            LOGGER_ERROR( "plugin '%s' not initialize"
                , this->getPluginName().c_str()
            );

            return false;
        }

        m_initializePlugin = true;

        m_systemPlugin = this->_systemPlugin();

        const ConstString & pluginName = this->getPluginName();

        NOTIFICATION_NOTIFY( NOTIFICATOR_PLUGIN_INITIALIZE, pluginName );

        LOGGER_INFO( "plugin", "plugin initialize '%s'"
            , this->getPluginName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PluginBase::finalizePlugin()
    {
        const ConstString & pluginName = this->getPluginName();

        SERVICE_PROVIDER_GET()
            ->unlinkService( pluginName.c_str() );

        if( m_initializePlugin == false )
        {
            return;
        }

        m_initializePlugin = false;

        NOTIFICATION_NOTIFY( NOTIFICATOR_PLUGIN_FINALIZE, pluginName );

        this->_finalizePlugin();

        LOGGER_INFO( "plugin", "plugin finalize '%s'"
            , this->getPluginName().c_str()
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
        MENGINE_ASSERTION_OBSERVABLE( this, "plugin '%s'"
            , this->getPluginName().c_str()
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
                    , this->getPluginName().c_str()
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
    bool PluginBase::addModuleFactory( const ConstString & _name, const FactoryInterfacePtr & _factory, const DocumentInterfacePtr & _doc )
    {
        VOCABULARY_SET( FactoryInterface, STRINGIZE_STRING_LOCAL( "Module" ), _name, _factory, _doc );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PluginBase::removeModuleFactory( const ConstString & _name )
    {
        FactoryInterfacePtr factory = VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Module" ), _name );

        MENGINE_ASSERTION_MEMORY_PANIC( factory, "not found module factory '%s'"
            , _name.c_str()
        );

        MENGINE_UNUSED( factory );
    }
    //////////////////////////////////////////////////////////////////////////
}