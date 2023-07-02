#include "PythonScriptService.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/ArchiveServiceInterface.h"
#include "Interface/PrefetcherServiceInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/ScriptEmbeddingInterface.h"
#include "Interface/ScriptProviderServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"
#include "Interface/DataServiceInterface.h"
#include "Interface/ThreadSystemInterface.h"
#include "Interface/DocumentInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonEventReceiver.h"

#include "PythonScriptModuleLoader.h"
#include "PythonEntityEventReceiver.h"

#include "DataflowPY.h"
#include "DataflowPYZ.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionVocabulary.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/EntityEventable.h"
#include "Kernel/Logger.h"
#include "Kernel/Error.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/Stringstream.h"
#include "Kernel/BuildMode.h"
#include "Kernel/StringHelper.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/FileGroupHelper.h"
#include "Kernel/OptionHelper.h"

#include "Config/StdString.h"
#include "Config/StdIO.h"
#include "Config/Algorithm.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ScriptService, Mengine::PythonScriptService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void s_pybind_logger( void * _user, const char * _msg )
        {
            MENGINE_UNUSED( _user );

            LOGGER_VERBOSE_LEVEL( "pybind", Mengine::LM_ERROR, Mengine::LFILTER_NONE, Mengine::LCOLOR_RED, nullptr, 0, LFLAG_SHORT )("%s"
                , _msg
                );
        }
        //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_DEBUG)
        //////////////////////////////////////////////////////////////////////////
#   if defined(MENGINE_WINDOWS_DEBUG)
        //////////////////////////////////////////////////////////////////////////
        static void py_invalid_parameter_handler( const wchar_t * _expression
            , const wchar_t * _function
            , const wchar_t * _file
            , unsigned int _line
            , uintptr_t pReserved )
        {
            MENGINE_UNUSED( pReserved );

            pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()
                ->getKernel();

            Char traceback[4096] = {'\0'};
            kernel->get_traceback( traceback, 4095 );

            MENGINE_ERROR_FATAL( "invalid parameter detected in function %ls.\nFile: %ls Line: %u\nExpression: %ls\nTrackeback:\n%s"
                , _function
                , _file
                , _line
                , _expression
                , traceback );
        }
        //////////////////////////////////////////////////////////////////////////
#   endif
        //////////////////////////////////////////////////////////////////////////
        class My_observer_bind_call
            : public pybind::observer_bind_call
        {
        public:
            explicit My_observer_bind_call( PythonScriptService * _scriptService )
                : m_scriptService( _scriptService )
#   if defined(MENGINE_WINDOWS_DEBUG)
                , m_prev_handler( nullptr )
                , m_prev_mode( 0 )
                , m_prev_handler_count( 0 )
#   endif
            {
            }

            virtual ~My_observer_bind_call()
            {
#   if defined(MENGINE_WINDOWS_DEBUG)
                MENGINE_ASSERTION_FATAL( m_prev_handler_count == 0 );
#   endif
            }

        public:
            void begin_bind_call( pybind::kernel_interface * _kernel, const char * _className, const char * _functionName, PyObject * _args, PyObject * _kwds ) override
            {
                MENGINE_UNUSED( _kernel );
                MENGINE_UNUSED( _kwds );
                MENGINE_UNUSED( _args );
                MENGINE_UNUSED( _functionName );
                MENGINE_UNUSED( _className );

                bool OPTION_pythoncalltrace = HAS_OPTION( "pythoncalltrace" );

                if( OPTION_pythoncalltrace == true )
                {
                    LOGGER_MESSAGE_RELEASE( "pybind call begin '%s::%s' args '%s' kwds '%s'"
                        , _className
                        , _functionName
                        , _kernel->object_repr( _args ).c_str()
                        , _kernel->object_repr( _kwds ).c_str()
                    );
                }

                uint32_t count = LOGGER_SERVICE()
                    ->getCountMessage( LM_ERROR );

                m_counts.emplace_back( count );

                m_scriptService
                    ->debugCallFunction( _className, _functionName, _args, _kwds );

#   if defined(MENGINE_WINDOWS_DEBUG)
                if( m_prev_handler_count++ == 0 )
                {
                    m_prev_handler = _set_invalid_parameter_handler( &Detail::py_invalid_parameter_handler );
                    m_prev_mode = _CrtSetReportMode( _CRT_ASSERT, 0 );
                }
#   endif
            }

            void end_bind_call( pybind::kernel_interface * _kernel, const char * _className, const char * _functionName, PyObject * _args, PyObject * _kwds ) override
            {
#   if defined(MENGINE_WINDOWS_DEBUG)
                if( --m_prev_handler_count == 0 )
                {
                    _set_invalid_parameter_handler( m_prev_handler );
                    _CrtSetReportMode( _CRT_ASSERT, m_prev_mode );
                }
#   endif

                bool OPTION_pythoncalltrace = HAS_OPTION( "pythoncalltrace" );

                if( OPTION_pythoncalltrace == true )
                {
                    LOGGER_MESSAGE_RELEASE( "pybind call end '%s::%s'"
                        , _className
                        , _functionName
                    );
                }

                uint32_t count = LOGGER_SERVICE()
                    ->getCountMessage( LM_ERROR );

                uint32_t last_count = m_counts.back();
                m_counts.pop_back();

                if( last_count == count )
                {
                    return;
                }

                if( MENGINE_STRCMP( _className, "PythonScriptLogger" ) == 0 )
                {
                    return;
                }

                if( MENGINE_STRCMP( _className, "ErrorScriptLogger" ) == 0 )
                {
                    return;
                }

                Stringstream ss_args;
                if( _args != nullptr )
                {
                    for( const pybind::object & obj : pybind::tuple( _kernel, _args ) )
                    {
                        ss_args << _kernel->object_repr( obj.ptr() ).c_str() << ", ";
                    }
                }

                Stringstream ss_kwds;
                if( _kwds != nullptr )
                {
                    for( const pybind::dict_pair_value & obj : pybind::dict( _kernel, _kwds ) )
                    {
                        pybind::object key = obj.key();
                        pybind::object value = obj.value();

                        ss_kwds << _kernel->object_repr( key.ptr() ).c_str() << " = " << _kernel->object_repr( value.ptr() ).c_str() << ", ";
                    }
                }

                LOGGER_VERBOSE_LEVEL( "script", LM_MESSAGE_RELEASE, LFILTER_NONE, LCOLOR_RED, nullptr, 0, LFLAG_SHORT )("script call '%s::%s' args [(%s)] kwds [(%s)] and get error!"
                    , _className
                    , _functionName
                    , ss_args.str().c_str()
                    , ss_kwds.str().c_str()
                    );

                Char traceback[4096] = {'\0'};
                _kernel->get_traceback( traceback, 4095 );

                LOGGER_VERBOSE_LEVEL( "script", LM_MESSAGE_RELEASE, LFILTER_NONE, LCOLOR_RED, nullptr, 0, LFLAG_SHORT )("traceback:\n%s"
                    , traceback
                    );
            }

        protected:
            PythonScriptService * m_scriptService;

            typedef Vector<uint32_t> VectorStackMsgCount;
            VectorStackMsgCount m_counts;

#   if defined(MENGINE_WINDOWS_DEBUG)
            _invalid_parameter_handler m_prev_handler;
            int m_prev_mode;
            uint32_t m_prev_handler_count;
#   endif
        };
        //////////////////////////////////////////////////////////////////////////
#endif
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    PythonScriptService::PythonScriptService()
        : m_kernel( nullptr )
        , m_moduleMengine( nullptr )
        , m_initializeModules( false )
        , m_tracebackOffset( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonScriptService::~PythonScriptService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonScriptService::_initializeService()
    {
        if( SERVICE_IS_INITIALIZE( ScriptProviderServiceInterface ) == false )
        {
            return false;
        }

        bool developmentMode = Helper::isDevelopmentMode();

        pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()
            ->getKernel();

        m_kernel = kernel;

        pybind::set_logger( (pybind::pybind_logger_t)&Detail::s_pybind_logger, nullptr );

        m_moduleMengine = this->initModule( "Mengine" );

        this->addGlobalModule( "Mengine", m_moduleMengine );

        kernel->set_currentmodule( m_moduleMengine );

        uint32_t python_version = kernel->get_python_version();
        this->addGlobalModuleT( "_PYTHON_VERSION", python_version );

        const Char * engineGITSHA1 = Helper::getEngineGITSHA1();
        this->addGlobalModuleT( "_ENGINE_GITSHA1", engineGITSHA1 );

        const Char * engineGITURL = Helper::getEngineGITURL();
        this->addGlobalModuleT( "_ENGINE_GITURL", engineGITURL );

        const Char * engineGITBranch = Helper::getEngineGITBranch();
        this->addGlobalModuleT( "_ENGINE_GITBRANCH", engineGITBranch );

        const Char * engineGITData = Helper::getEngineGITDate();
        this->addGlobalModuleT( "_ENGINE_GITDATA", engineGITData );

        const Char * contentCommit = Helper::getContentCommit();
        this->addGlobalModuleT( "_CONTENT_COMMIT", contentCommit );

        bool publish = MENGINE_BUILD_PUBLISH_VALUE( true, false );

        if( publish == true )
        {
            this->addGlobalModuleT( "_BUILD_MODE", "publish" );
        }
        else
        {
            const Char * buildMode = MENGINE_MASTER_RELEASE_VALUE( "alpha", "develop" );

            this->addGlobalModuleT( "_BUILD_MODE", buildMode );
        }
        
        const Char * buildProjectName = Helper::getBuildProjectName();
        this->addGlobalModuleT( "_BUILD_PROJECTNAME", buildProjectName );

        const Char * buildTimestamp = Helper::getBuildTimestamp();
        this->addGlobalModuleT( "_BUILD_TIMESTAMP", buildTimestamp );

        const Char * buildUsername = Helper::getBuildUsername();
        this->addGlobalModuleT( "_BUILD_USERNAME", buildUsername );

        const Char * buildVersion = Helper::getBuildVersion();
        this->addGlobalModuleT( "_BUILD_VERSION", buildVersion );

        uint32_t buildVersionMajor = Helper::getBuildVersionMajor();
        this->addGlobalModuleT( "_BUILD_VERSION_MAJOR", buildVersionMajor );

        uint32_t buildVersionMinor = Helper::getBuildVersionMinor();
        this->addGlobalModuleT( "_BUILD_VERSION_MINOR", buildVersionMinor );

        uint32_t buildVersionPatch = Helper::getBuildVersionPatch();
        this->addGlobalModuleT( "_BUILD_VERSION_PATCH", buildVersionPatch );

        uint32_t buildVersionNumber = Helper::getBuildVersionNumber();
        this->addGlobalModuleT( "_BUILD_VERSION_NUMBER", buildVersionNumber );

        uint64_t buildNumber = Helper::getBuildNumber();
        this->addGlobalModuleT( "_BUILD_NUMBER", buildNumber );

        m_availablePlugins = pybind::make_dict_t( kernel );

        this->addGlobalModule( "_PLUGINS", m_availablePlugins.ptr() );

        pybind::def_functor( m_kernel, "setAvailablePlugin", this, &PythonScriptService::setAvailablePlugin );
        pybind::def_functor( m_kernel, "isAvailablePlugin", this, &PythonScriptService::isAvailablePlugin );

        pybind::def_functor( m_kernel, "setTracebackOffset", this, &PythonScriptService::setTracebackOffset );

        pybind::interface_<PythonScriptLogger>( m_kernel, "PythonScriptLogger", true )
            .def_native_silent_kernel( "write", &PythonScriptLogger::py_write )
            .def_property( "softspace", &PythonScriptLogger::getSoftspace, &PythonScriptLogger::setSoftspace )
            ;

        m_loggerWarning = Helper::makeFactorableUnique<PythonScriptLogger>( MENGINE_DOCUMENT_FACTORABLE );

        m_loggerWarning->setVerboseLevel( LM_WARNING );
        m_loggerWarning->setColor( LCOLOR_RED | LCOLOR_GREEN );

        pybind::object py_logger = pybind::make_object_t( m_kernel, m_loggerWarning );
        kernel->setStdOutHandle( py_logger.ptr() );

        m_loggerError = Helper::makeFactorableUnique<PythonScriptLogger>( MENGINE_DOCUMENT_FACTORABLE );

        m_loggerError->setVerboseLevel( LM_ERROR );
        m_loggerError->setColor( LCOLOR_RED );

        pybind::object py_loggerError = pybind::make_object_t( m_kernel, m_loggerError );
        kernel->setStdErrorHandle( py_loggerError.ptr() );

#if defined(MENGINE_DEBUG)
        pybind::observer_bind_call * bind_call = Helper::newT<Detail::My_observer_bind_call>( this );
        m_kernel->set_observer_bind_call( bind_call );
#endif

        DataflowPYPtr dataflowPY = Helper::makeFactorableUnique<DataflowPY>( MENGINE_DOCUMENT_FACTORABLE );

        dataflowPY->setKernel( m_kernel );

        if( dataflowPY->initialize() == false )
        {
            return false;
        }

        VOCABULARY_SET( DataflowInterface, STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "pyScript" ), dataflowPY, MENGINE_DOCUMENT_FACTORABLE );

        DataflowPYZPtr dataflowPYZ = Helper::makeFactorableUnique<DataflowPYZ>( MENGINE_DOCUMENT_FACTORABLE );

        dataflowPYZ->setKernel( m_kernel );

        ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "zip" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( archivator );

        dataflowPYZ->setArchivator( archivator );

        if( dataflowPYZ->initialize() == false )
        {
            return false;
        }

        VOCABULARY_SET( DataflowInterface, STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "pyzScript" ), dataflowPYZ, MENGINE_DOCUMENT_FACTORABLE );

        SERVICE_LEAVE( DataServiceInterface, []()
        {
            DataflowInterfacePtr dataflowPy = VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "pyScript" ) );
            dataflowPy->finalize();

            DataflowInterfacePtr dataflowPyz = VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "pyzScript" ) );
            dataflowPyz->finalize();
        } );

        pybind::interface_<PythonScriptModuleFinder>( m_kernel, "PythonScriptModuleFinder", true )
            .def_kernel( "find_module", &PythonScriptModuleFinder::find_module )
            .def_kernel( "load_module", &PythonScriptModuleFinder::load_module )
            ;

        m_moduleFinder = Helper::makeFactorableUnique<PythonScriptModuleFinder>( MENGINE_DOCUMENT_FACTORABLE );

        m_moduleFinder->setDataflowPY( dataflowPY );
        m_moduleFinder->setDataflowPYZ( dataflowPYZ );

        if( m_moduleFinder->initialize() == false )
        {
            LOGGER_ERROR( "invalid initialize module finder" );

            return false;
        }

        pybind::object py_moduleFinder = pybind::make_object_t( m_kernel, m_moduleFinder );

        m_moduleFinder->setEmbed( py_moduleFinder );

        kernel->set_module_finder( py_moduleFinder.ptr() );

        m_factoryScriptModule = Helper::makeFactoryPool<PythonScriptModule, 8>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryEntityEventable = Helper::makeFactoryPool<EntityEventable, 64>( MENGINE_DOCUMENT_FACTORABLE );

#if defined(MENGINE_DEBUG)
        pybind::def_functor( m_kernel, "addDebugLogFunction", this, &PythonScriptService::addDebugLogFunction );
        pybind::def_functor( m_kernel, "removeDebugLogFunction", this, &PythonScriptService::removeDebugLogFunction );
#endif

        pybind::def_functor( m_kernel, "getModuleFinderPaths", this, &PythonScriptService::getModuleFinderPaths );
        pybind::def_functor( m_kernel, "getModuleFinderFileGroups", this, &PythonScriptService::getModuleFinderFileGroups );

        this->addGlobalModuleT( "_DEVELOPMENT", developmentMode );

        bool qualityAssuranceMode = Helper::isQualityAssuranceMode();

        this->addGlobalModuleT( "_QUALITYASSURANCE", qualityAssuranceMode );

        const Char * option_globals[MENGINE_OPTIONS_VALUES_MAX] = {nullptr};
        uint32_t option_globals_count = 0;
        if( OPTIONS_SERVICE()
            ->getOptionValues( "globals", option_globals, &option_globals_count ) == true )
        {
            this->addGlobalModuleT( "_DEBUG", false );
            this->addGlobalModuleT( "_WINDOWS", false );
            this->addGlobalModuleT( "_WIN32", false );
            this->addGlobalModuleT( "_WIN64", false );
            this->addGlobalModuleT( "_MACOS", false );
            this->addGlobalModuleT( "_DESKTOP", false );
            this->addGlobalModuleT( "_LINUX", false );
            this->addGlobalModuleT( "_ANDROID", false );
            this->addGlobalModuleT( "_IOS", false );
            this->addGlobalModuleT( "_MOBILE", false );
            this->addGlobalModuleT( "_MASTER_RELEASE", false );
            this->addGlobalModuleT( "_BUILD_PUBLISH", false );

            for( uint32_t index = 0; index != option_globals_count; ++index )
            {
                const Char * option_global = option_globals[index];

                Char uppercase_option_global[256] = {'\0'};
                Helper::toupper( option_global, uppercase_option_global, 256 );

                Char total_global[256] = {'\0'};
                MENGINE_SNPRINTF( total_global, 256, "_%s", uppercase_option_global );

                this->addGlobalModuleT( total_global, true );
            }
        }
        else
        {
#if defined(MENGINE_DEBUG)
            this->addGlobalModuleT( "_DEBUG", true );
#else
            this->addGlobalModuleT( "_DEBUG", false );
#endif

#if defined(MENGINE_PLATFORM_WINDOWS)
            this->addGlobalModuleT( "_WINDOWS", true );
#else
            this->addGlobalModuleT( "_WINDOWS", false );
#endif

#if defined(MENGINE_PLATFORM_WINDOWS32)
            this->addGlobalModuleT( "_WIN32", true );
#else
            this->addGlobalModuleT( "_WIN32", false );
#endif

#if defined(MENGINE_PLATFORM_WINDOWS64)
            this->addGlobalModuleT( "_WIN64", true );
#else
            this->addGlobalModuleT( "_WIN64", false );
#endif

#if defined(MENGINE_PLATFORM_MACOS)
            this->addGlobalModuleT( "_MACOS", true );
#else
            this->addGlobalModuleT( "_MACOS", false );
#endif

#if defined(MENGINE_PLATFORM_DESKTOP)
            this->addGlobalModuleT( "_DESKTOP", true );
#else
            this->addGlobalModuleT( "_DESKTOP", false );
#endif

#if defined(MENGINE_PLATFORM_LINUX)
            this->addGlobalModuleT( "_LINUX", true );
#else
            this->addGlobalModuleT( "_LINUX", false );
#endif

#if defined(MENGINE_PLATFORM_ANDROID)
            this->addGlobalModuleT( "_ANDROID", true );
#else
            this->addGlobalModuleT( "_ANDROID", false );
#endif

#if defined(MENGINE_PLATFORM_IOS)
            this->addGlobalModuleT( "_IOS", true );
#else
            this->addGlobalModuleT( "_IOS", false );
#endif

#if defined(MENGINE_PLATFORM_APPLE)
            this->addGlobalModuleT( "_APPLE", true );
#else
            this->addGlobalModuleT( "_APPLE", false );
#endif

#if defined(MENGINE_PLATFORM_IOS_SIMULATOR)
            this->addGlobalModuleT( "_IOS_SIMULATOR", true );
#else
            this->addGlobalModuleT( "_IOS_SIMULATOR", false );
#endif

#if defined(MENGINE_PLATFORM_MOBILE)
            this->addGlobalModuleT( "_MOBILE", true );
#else
            this->addGlobalModuleT( "_MOBILE", false );
#endif

#if defined(MENGINE_MASTER_RELEASE)
            this->addGlobalModuleT( "_MASTER_RELEASE", true );
#else
            this->addGlobalModuleT( "_MASTER_RELEASE", false );
#endif

#if defined(MENGINE_BUILD_PUBLISH)
            this->addGlobalModuleT( "_BUILD_PUBLISH", true );
#else
            this->addGlobalModuleT( "_BUILD_PUBLISH", false );
#endif
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptService::_finalizeService()
    {
        m_availablePlugins = nullptr;

#if defined(MENGINE_DEBUG)
        pybind::observer_bind_call * observer = m_kernel->get_observer_bind_call();

        Detail::My_observer_bind_call * observer_bind_call = static_cast<Detail::My_observer_bind_call *>(observer);
        Helper::deleteT( static_cast<Detail::My_observer_bind_call *>(observer_bind_call) );

        m_kernel->set_observer_bind_call( nullptr );

        m_debugCallFunctions.clear();
#endif

#if defined(MENGINE_DEBUG)
        for( const ScriptEmbeddingDesc & desc : m_embeddings )
        {
            const ScriptEmbeddingInterfacePtr & embedding = desc.embedding;

            LOGGER_ERROR( "was forgotten ejecting '%s'"
                , desc.name.c_str()
            );

            embedding->eject( m_kernel );
        }
#endif

        m_embeddings.clear();

        m_kernel->remove_module_finder();
                
        this->removeGlobalModule( "Mengine" );
        this->removeGlobalModule( "_PLUGINS" );

        MENGINE_ASSERTION_VOCABULARY_EMPTY( STRINGIZE_STRING_LOCAL( "ScriptWrapping" ) );

        if( m_moduleFinder != nullptr )
        {
            m_moduleFinder->finalize();
            m_moduleFinder = nullptr;
        }

        m_loggerWarning = nullptr;
        m_loggerError = nullptr;

        m_kernel->setStdOutHandle( nullptr );
        m_kernel->setStdErrorHandle( nullptr );

        m_kernel->remove_scope<PythonScriptLogger>();
        m_kernel->remove_scope<PythonScriptModuleFinder>();

        m_kernel->module_fini( m_moduleMengine );
        m_moduleMengine = nullptr;

        m_kernel->set_current_module( nullptr );

        m_bootstrapperModules.clear();
        m_prototypies.clear();

        for( IntrusiveListConstStringHolderPythonString::iterator it = m_holdersPythonString.begin(); it != m_holdersPythonString.end(); )
        {
            IntrusiveListConstStringHolderPythonString::iterator it_erase = it;

            ConstStringHolderPythonString * holder = *it;
            ++it;

            m_holdersPythonString.erase( it_erase );

            m_poolPythonString.destroyT( holder );
        }

        m_holdersPythonString.clear();
        m_poolPythonString.clear();

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryScriptModule );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryEntityEventable );

        m_factoryScriptModule = nullptr;
        m_factoryEntityEventable = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonScriptService::_runService()
    {
#if defined(MENGINE_DEBUG)
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_ASSERTION, &PythonScriptService::notifyAssertion_, MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptService::_stopService()
    {
#if defined(MENGINE_DEBUG)
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_ASSERTION );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptService::addModulePath( const FileGroupInterfacePtr & _fileGroup, const VectorScriptModulePack & _modules )
    {
        if( _modules.empty() == true )
        {
            return;
        }

        VectorFilePath pathes;

        for( const ScriptModulePackage & pack : _modules )
        {
            pathes.emplace_back( pack.path );
        }

        for( const FilePath & filePath : pathes )
        {
            LOGGER_INFO( "script", "add module path '%s'"
                , Helper::getFileGroupFullPath( _fileGroup, filePath )
            );
        }

        m_moduleFinder->addModulePath( _fileGroup, pathes );

        m_bootstrapperModules.insert( m_bootstrapperModules.end(), _modules.begin(), _modules.end() );

        if( m_initializeModules == true )
        {
            for( const ScriptModulePackage & pack : _modules )
            {
                this->initializeModule_( pack );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptService::removeModulePath( const FileGroupInterfacePtr & _fileGroup, const VectorScriptModulePack & _modules )
    {
        m_moduleFinder->removeModulePath( _fileGroup );

        for( const ScriptModulePackage & pack : _modules )
        {
            m_bootstrapperModules.erase(
                Algorithm::remove_if( m_bootstrapperModules.begin(), m_bootstrapperModules.end(), [&pack]( const ScriptModulePackage & _pack )
            {
                if( _pack.module < pack.module )
                {
                    return false;
                }

                if( _pack.path < pack.path )
                {
                    return false;
                }

                return true;
            } )
                , m_bootstrapperModules.end()
                );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonScriptService::addScriptEmbedding( const ConstString & _name, const ScriptEmbeddingInterfacePtr & _embedding )
    {
        LOGGER_INFO( "script", "add script embedding '%s'"
            , _name.c_str()
        );

        MENGINE_ASSERTION_FATAL( Algorithm::find_if( m_embeddings.begin(), m_embeddings.end(), [&_name]( const ScriptEmbeddingDesc & _desc )
        {
            return _desc.name == _name;
        } ) == m_embeddings.end() );

        ScriptEmbeddingDesc desc;
        desc.name = _name;
        desc.embedding = _embedding;

        if( _embedding->embed( m_kernel ) == false )
        {
            return false;
        }

        m_embeddings.emplace_back( desc );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptService::removeScriptEmbedding( const ConstString & _name )
    {
        VectorEmbeddings::const_iterator it_found = Algorithm::find_if( m_embeddings.begin(), m_embeddings.end(), [&_name]( const ScriptEmbeddingDesc & _desc )
        {
            return _desc.name == _name;
        } );

        MENGINE_ASSERTION_FATAL( it_found != m_embeddings.end() );

        const ScriptEmbeddingDesc & desc = *it_found;

        const ScriptEmbeddingInterfacePtr & embedding = desc.embedding;

        embedding->eject( m_kernel );

        m_kernel->collect();

        m_embeddings.erase( it_found );
    }
    //////////////////////////////////////////////////////////////////////////
    EventablePtr PythonScriptService::eventableEntity( const pybind::object & _type )
    {
        if( _type.is_invalid() == true || _type.is_none() == true )
        {
            LOGGER_ERROR( "type invalid" );

            return nullptr;
        }

        if( _type.is_type_class() == true )
        {
            PyObject * py_type_ptr = _type.ptr();

            if( m_kernel->type_initialize( py_type_ptr ) == false )
            {
                LOGGER_ERROR( "type '%s' invalid initialize"
                    , _type.repr().c_str()
                );

                return nullptr;
            }
        }

        EntityEventablePtr eventable = m_factoryEntityEventable->createObject( MENGINE_DOCUMENT_FACTORABLE );

        Helper::registerPythonEventReceiverMethod<PythonEntityEventReceiver>( m_kernel, _type, eventable, STRINGIZE_STRING_LOCAL( "onCreate" ), EVENT_ENTITY_CREATE, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerPythonEventReceiverMethod<PythonEntityEventReceiver>( m_kernel, _type, eventable, STRINGIZE_STRING_LOCAL( "onDestroy" ), EVENT_ENTITY_DESTROY, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerPythonEventReceiverMethod<PythonEntityEventReceiver>( m_kernel, _type, eventable, STRINGIZE_STRING_LOCAL( "onPreparation" ), EVENT_ENTITY_PREPARATION, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerPythonEventReceiverMethod<PythonEntityEventReceiver>( m_kernel, _type, eventable, STRINGIZE_STRING_LOCAL( "onActivate" ), EVENT_ENTITY_ACTIVATE, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerPythonEventReceiverMethod<PythonEntityEventReceiver>( m_kernel, _type, eventable, STRINGIZE_STRING_LOCAL( "onPreparationDeactivate" ), EVENT_ENTITY_PREPARATION_DEACTIVATE, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerPythonEventReceiverMethod<PythonEntityEventReceiver>( m_kernel, _type, eventable, STRINGIZE_STRING_LOCAL( "onDeactivate" ), EVENT_ENTITY_DEACTIVATE, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerPythonEventReceiverMethod<PythonEntityEventReceiver>( m_kernel, _type, eventable, STRINGIZE_STRING_LOCAL( "onCompile" ), EVENT_ENTITY_COMPILE, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerPythonEventReceiverMethod<PythonEntityEventReceiver>( m_kernel, _type, eventable, STRINGIZE_STRING_LOCAL( "onRelease" ), EVENT_ENTITY_RELEASE, MENGINE_DOCUMENT_FACTORABLE );

        return eventable;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonScriptService::bootstrapModules()
    {
        for( const ScriptModulePackage & package : m_bootstrapperModules )
        {
            if( package.module.empty() == true )
            {
                continue;
            }

            ScriptModuleInterfacePtr module = this->importModule( package.module );

            if( module == nullptr )
            {
                LOGGER_ERROR( "invalid import module '%s'"
                    , package.module.c_str()
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonScriptService::initializeModules()
    {
        if( m_initializeModules == true )
        {
            return false;
        }

        for( const ScriptModulePackage & package : m_bootstrapperModules )
        {
            if( this->initializeModule_( package ) == false )
            {
                return false;
            }
        }

        m_initializeModules = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonScriptService::initializeModule_( const ScriptModulePackage & _pack )
    {
        if( _pack.module.empty() == true )
        {
            return true;
        }

        ScriptModuleInterfacePtr module = this->importModule( _pack.module );

        if( module == nullptr )
        {
            return false;
        }

        if( _pack.initializer.empty() == true )
        {
            return true;
        }

        if( module->onInitialize( _pack.initializer ) == false )
        {
            LOGGER_ERROR( "module '%s' invalid initialize"
                , _pack.module.c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonScriptService::finalizeModules()
    {
        if( m_initializeModules == false )
        {
            return true;
        }

        m_initializeModules = false;

        for( const ScriptModulePackage & package : m_bootstrapperModules )
        {
            if( package.module.empty() == true )
            {
                continue;
            }

            if( package.finalizer.empty() == true )
            {
                continue;
            }

            ScriptModuleInterfacePtr module = this->importModule( package.module );

            MENGINE_ASSERTION_MEMORY_PANIC( module, "invalid import module '%s'"
                , package.module.c_str()
            );

            if( module->onFinalize( package.finalizer ) == false )
            {
                LOGGER_ERROR( "module '%s' invalid call finalizer"
                    , package.module.c_str()
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptService::prefetchModules( const PrefetcherObserverInterfacePtr & _cb )
    {
        DataflowInterfacePtr dataflowPY = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "pyScript" ) );

        DataflowInterfacePtr dataflowPYZ = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "pyzScript" ) );

        for( const ScriptModulePackage & pack : m_bootstrapperModules )
        {
            const FileGroupInterfacePtr & fileGroup = pack.fileGroup;

            fileGroup->findFiles( pack.path, "*.pyz", [&fileGroup, &dataflowPYZ, &_cb]( const FilePath & _filePath )
            {
                DataflowContext context;
                context.filePath = _filePath;

                if( PREFETCHER_SERVICE()
                    ->prefetchData( fileGroup, _filePath, dataflowPYZ, &context, _cb ) == false )
                {
                    LOGGER_ERROR( "invalid prefetch data '%s'"
                        , _filePath.c_str()
                    );
                }

                return true;
            } );

            static bool PythonScript_AvailableSourceCode = CONFIG_VALUE( "PythonScript", "AvailableSourceCode", true );

            if( PythonScript_AvailableSourceCode == true )
            {
                fileGroup->findFiles( pack.path, "*.py", [&fileGroup, &dataflowPY, &_cb]( const FilePath & _filePath )
                {
                    DataflowContext context;
                    context.filePath = _filePath;

                    if( PREFETCHER_SERVICE()
                        ->prefetchData( fileGroup, _filePath, dataflowPY, &context, _cb ) == false )
                    {
                        LOGGER_ERROR( "invalid prefetch data '%s'"
                            , _filePath.c_str()
                        );
                    }

                    return true;
                } );
            }
        }

        _cb->onPrefetchPreparation();
        _cb->onPrefetchComplete( true );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * PythonScriptService::initModule( const Char * _name )
    {
        LOGGER_INFO( "script", "init module '%s'"
            , _name
        );

        PyObject * module = m_kernel->module_init( _name );

        return module;
    }
    //////////////////////////////////////////////////////////////////////////
    ScriptModuleInterfacePtr PythonScriptService::importModule( const ConstString & _name )
    {
        LOGGER_INFO( "script", "import module '%s'"
            , _name.c_str()
        );

        PyObject * py_module = nullptr;
        bool exist = false;

        try
        {
            py_module = m_kernel->module_import( _name.c_str(), exist );
        }
        catch( ... )
        {
            LOGGER_ERROR( "invalid import module '%s' (c-exception)"
                , _name.c_str()
            );

            return nullptr;
        }

        if( exist == false )
        {
            LOGGER_WARNING( "invalid import module '%s' (not exist)"
                , _name.c_str()
            );

            return nullptr;
        }

        MENGINE_ASSERTION_MEMORY_PANIC( py_module, "invalid import module '%s' (script)"
            , _name.c_str()
        );

        ScriptModulePtr module = m_factoryScriptModule->createObject( MENGINE_DOCUMENT_FACTORABLE );

        if( module->initialize( pybind::module( m_kernel, py_module ) ) == false )
        {
            LOGGER_ERROR( "invalid import initialize '%s' (script)"
                , _name.c_str()
            );

            return nullptr;
        }

        return module;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptService::addGlobalModule( const Char * _name, PyObject * _module )
    {
        LOGGER_INFO( "script", "add global module '%s' object '%s'"
            , _name
            , m_kernel->object_repr( _module ).c_str()
        );

        PyObject * builtins = m_kernel->get_builtins();

        PyObject * dir_bltin = m_kernel->module_dict( builtins );

        pybind::dict_set_t( m_kernel, dir_bltin, _name, _module );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * PythonScriptService::getGlobalModule( const Char * _name ) const
    {
        PyObject * builtins = m_kernel->get_builtins();

        PyObject * dir_bltin = m_kernel->module_dict( builtins );

        PyObject * module = pybind::dict_get_t( m_kernel, dir_bltin, _name );

        return module;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptService::removeGlobalModule( const Char * _name )
    {
        PyObject * builtins = m_kernel->get_builtins();

        PyObject * dir_bltin = m_kernel->module_dict( builtins );

        pybind::dict_remove_t( m_kernel, dir_bltin, _name );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptService::setAvailablePlugin( const Char * _name, bool _available )
    {
        m_availablePlugins[_name] = _available;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonScriptService::isAvailablePlugin( const Char * _name ) const
    {
        if( m_availablePlugins.exist( _name ) == false )
        {
            return false;
        }

        bool available = m_availablePlugins.get( _name );

        return available;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptService::stringize( PyObject * _object, ConstString * const _cstr )
    {
#if defined(MENGINE_DEBUG)
        if( m_kernel->string_check( _object ) == false )
        {
            LOGGER_FATAL( "invalid stringize object '%s' not string type '%s'"
                , m_kernel->object_repr( _object ).c_str()
                , m_kernel->object_repr_type( _object ).c_str()
            );

            return;
        }
#endif

        if( m_kernel->string_size( _object ) == 0 )
        {
            *_cstr = ConstString::none();

            return;
        }

        ConstStringHolderPythonString * holder = m_poolPythonString.createT();

        holder->setPythonObject( m_kernel, (PyObject *)_object );

        if( STRINGIZE_SERVICE()
            ->stringizeExternal( holder, _cstr ) == false )
        {
            m_poolPythonString.destroyT( holder );

            return;
        }

        m_holdersPythonString.push_back( holder );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptService::setTracebackOffset( uint32_t _tracebackOffset )
    {
        m_tracebackOffset = _tracebackOffset;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t PythonScriptService::getTracebackOffset() const
    {
        return m_tracebackOffset;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptService::notifyAssertion_( const Char * _category, EAssertionLevel _level, const Char * _test, const Char * _file, int32_t _line, const Char * _message )
    {
        MENGINE_UNUSED( _category );
        MENGINE_UNUSED( _level );
        MENGINE_UNUSED( _test );
        MENGINE_UNUSED( _file );
        MENGINE_UNUSED( _line );
        MENGINE_UNUSED( _message );

        ThreadId scriptThreadId = SCRIPTPROVIDER_SERVICE()
            ->getScriptThreadId();

        ThreadId currentThreadId = THREAD_SYSTEM()
            ->getCurrentThreadId();

        if( scriptThreadId != currentThreadId )
        {
            return;
        }

        Char traceback[MENGINE_LOGGER_MAX_MESSAGE] = {'\0'};
        if( m_kernel->get_traceback( traceback, MENGINE_LOGGER_MAX_MESSAGE ) == false )
        {
            return;
        }

        LOGGER_CATEGORY_VERBOSE_LEVEL( Mengine::LM_MESSAGE_RELEASE, Mengine::LFILTER_NONE, Mengine::LCOLOR_RED, nullptr, 0, Mengine::LFLAG_SHORT )("traceback:\n%s", traceback);
    }
    //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_DEBUG)
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptService::addDebugLogFunction( const ConstString & _className, const ConstString & _functionName, const pybind::object & _filter )
    {
        DebugCallDesc desc;
        desc.className = _className;
        desc.functionName = _functionName;
        desc.filter = _filter;

        m_debugCallFunctions.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptService::removeDebugLogFunction( const ConstString & _className, const ConstString & _functionName )
    {
        VectorDebugCallFunctions::iterator it_found = Algorithm::find_if( m_debugCallFunctions.begin(), m_debugCallFunctions.end(), [&_className, &_functionName]( const DebugCallDesc & _desc )
        {
            if( _desc.className != _className )
            {
                return false;
            }

            if( _desc.functionName != _functionName )
            {
                return false;
            }

            return true;
        } );

        m_debugCallFunctions.erase( it_found );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptService::debugCallFunction( const Char * _className, const Char * _functionName, PyObject * _args, PyObject * _kwds )
    {
        for( const DebugCallDesc & _desc : m_debugCallFunctions )
        {
            if( _desc.className != _className )
            {
                continue;
            }

            if( _desc.functionName != _functionName )
            {
                continue;
            }

            if( _desc.filter.is_callable() == true )
            {
                PyObject * ret_filter = m_kernel->ask_native( _desc.filter.ptr(), _args );

                pybind::object scope_ret_filter( m_kernel, ret_filter, pybind::borrowed );

                if( m_kernel->bool_check( ret_filter ) != true || m_kernel->is_true( ret_filter ) != true )
                {
                    continue;
                }
            }

            if( _desc.className.empty() == true )
            {
                LOGGER_STATISTIC( "Debug call '%s' args '%s' kwds '%s':"
                    , _functionName
                    , m_kernel->object_repr( _args ).c_str()
                    , m_kernel->object_repr( _kwds ).c_str()
                );

                Char traceback[4096] = {'\0'};
                m_kernel->get_traceback( traceback, 4095 );

                LOGGER_STATISTIC( "%s", traceback );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
#endif
    //////////////////////////////////////////////////////////////////////////
    pybind::list PythonScriptService::getModuleFinderPaths() const
    {
        pybind::list l( m_kernel );

        m_moduleFinder->foreachModulePaths( [&l]( const ModulePathes & _desc )
        {
            const FileGroupInterfacePtr & fileGroup = _desc.fileGroup;

            for( const FilePath & path : _desc.pathes )
            {
                Char fullpath[MENGINE_MAX_PATH] = {'\0'};
                fileGroup->getFullPath( path, fullpath );

                l.append( fullpath );
            }
        } );

        return l;
    }
    //////////////////////////////////////////////////////////////////////////
    pybind::list PythonScriptService::getModuleFinderFileGroups() const
    {
        pybind::list l( m_kernel );

        m_moduleFinder->foreachModulePaths( [&l]( const ModulePathes & _desc )
        {
            const FileGroupInterfacePtr & fileGroup = _desc.fileGroup;

            l.append( fileGroup );
        } );

        return l;
    }
    //////////////////////////////////////////////////////////////////////////
}
