#include "PluginBase.h"

#include "Interface/ThreadServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/FactoryServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"

#ifdef MENGINE_PLATFORM_WINDOWS
#   include "Kernel/Win32Helper.h"
#endif

#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PluginBase::PluginBase()
        : m_uid( 0 )
        , m_dynamicLoad( false )
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
        const Char * pluginName = this->getPluginName();
        MENGINE_UNUSED( pluginName );

        this->_destroyPlugin();
        this->_destroy();

        bool dynamicLoad = m_dynamicLoad;

        Helper::deleteT( this );

#if defined(MENGINE_WINDOWS_DEBUG)
        if( dynamicLoad == true )
        {
            const Char * plugin_modulePath = Helper::Win32GetCurrentDllPath();

            typedef Vector<DocumentPtr> VectorDocuments;
            VectorDocuments leakObjects;

            FACTORY_SERVICE()
                ->visitFactoryLeakObjects( ~0U, [plugin_modulePath, &leakObjects]( const Factory * _factory, const Factorable * _factorable, const Char * _type, const DocumentPtr & _doc )
            {
                MENGINE_UNUSED( _factory );
                MENGINE_UNUSED( _factorable );
                MENGINE_UNUSED( _type );

                if( _doc == nullptr )
                {
                    return;
                }

                const Char * object_modulePath = _doc->getModulePath();

                if( MENGINE_STRCMP( plugin_modulePath, object_modulePath ) != 0 )
                {
                    return;
                }

                leakObjects.emplace_back( _doc );
            } );

            if( leakObjects.empty() == false )
            {
                LOGGER_MESSAGE( "Plugin[%s] leak %u objects"
                    , pluginName
                    , leakObjects.size()
                );

                for( const DocumentPtr & doc : leakObjects )
                {
                    LOGGER_MESSAGE( "-- %s"
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
    bool PluginBase::addModuleFactory( const ConstString & _name, const ModuleFactoryInterfacePtr & _factory )
    {
        VOCABULARY_SET( ModuleFactoryInterface, STRINGIZE_STRING_LOCAL( "Module" ), _name, _factory );

        m_moduleFactories.emplace_back( _name );

        return true;
    }
}