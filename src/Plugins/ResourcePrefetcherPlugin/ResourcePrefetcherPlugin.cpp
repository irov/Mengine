#include "ResourcePrefetcherPlugin.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/ArchiveServiceInterface.h"
#include "Interface/TextServiceInterface.h"

#include "ResourcePrefetcherServiceInterface.h"

#include "DataflowResourcePrefetcher.h"
#include "ArchiveResourcePrefetcher.h"
#include "ImageDecoderResourcePrefetcher.h"
#include "SoundDecoderResourcePrefetcher.h"
#include "DefaultResourcePrefetcher.h"

#include "Kernel/FactorableUnique.h"

#include "pybind/pybind.hpp"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( ResourcePrefetcherService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( ResourcePrefetcher, Mengine::ResourcePrefetcherPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        class PyPrefetcherObserver
            : public PrefetcherObserverInterface
            , public Factorable
        {
        public:
            PyPrefetcherObserver( const pybind::object & _cb, const pybind::args & _args )
                : m_cb( _cb )
                , m_args( _args )
                , m_count( 0 )
                , m_successful( true )
            {
            }

            ~PyPrefetcherObserver() override
            {
            }

        public:
            uint32_t getCount() const
            {
                return m_count;
            }

        protected:
            void onPrefetchIgnored() override
            {
            }

            void onPrefetchAlreadyExist() override
            {
            }

            void onPrefetchPreparation() override
            {
                ++m_count;
            }

            bool onPrefetchRun() override
            {
                return true;
            }

            void onPrefetchCancel() override
            {
                m_successful = false;

                if( --m_count == 0 )
                {
                    m_cb.call_args( false, m_args );
                }
            }

            void onPrefetchComplete( bool _successful ) override
            {
                m_successful &= _successful;

                if( --m_count == 0 )
                {
                    m_cb.call_args( m_successful, m_args );
                }
            }

        protected:
            pybind::object m_cb;
            pybind::args m_args;

            uint32_t m_count;
            bool m_successful;
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<PyPrefetcherObserver> PyPrefetcherObserverPtr;
        //////////////////////////////////////////////////////////////////////////
        static bool s_prefetchResources( const ConstString & _category, const ConstString & _groupName, const pybind::object & _cb, const pybind::args & _args )
        {
            const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
                ->getFileGroup( _category );

            if( fileGroup == nullptr )
            {
                return false;
            }

            PyPrefetcherObserverPtr observer = Helper::makeFactorableUnique<PyPrefetcherObserver>( _cb, _args );

            RESOURCE_SERVICE()
                ->foreachGroupResources( fileGroup, _groupName, [observer]( const ResourcePtr & _resource )
            {
                RESOURCEPREFETCHER_SERVICE()
                    ->prefetchResource( _resource, observer );
            } );

            uint32_t count = observer->getCount();

            return count != 0;
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_unfetchResources( const ConstString & _category, const ConstString & _groupName )
        {
            const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
                ->getFileGroup( _category );

            if( fileGroup == nullptr )
            {
                return;
            }

            RESOURCE_SERVICE()
                ->foreachGroupResources( fileGroup, _groupName, []( const ResourcePtr & _resource )
            {
                RESOURCEPREFETCHER_SERVICE()
                    ->unfetchResource( _resource );
            } );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_prefetchFonts( const pybind::object & _cb, const pybind::args & _args )
        {
            PyPrefetcherObserverPtr observer = Helper::makeFactorableUnique<PyPrefetcherObserver>( _cb, _args );

            TEXT_SERVICE()
                ->foreachFonts( [observer]( const TextFontInterfacePtr & _textFont )
            {
                _textFont->prefetch( observer );
            } );

            uint32_t count = observer->getCount();

            return count != 0;
        }
        //////////////////////////////////////////////////////////////////////////
        class PyMutexPrefetcherObserver
            : public PyPrefetcherObserver
        {
        public:
            PyMutexPrefetcherObserver( pybind::kernel_interface * _kernel, const pybind::object & _cb, const pybind::args & _args )
                : PyPrefetcherObserver( _cb, _args )
                , m_kernel( _kernel )
            {
                m_kernel->acquire_mutex();
            }

            ~PyMutexPrefetcherObserver() override
            {
                m_kernel->release_mutex();
            }

        protected:
            pybind::kernel_interface * m_kernel;
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<PyPrefetcherObserver> PyPrefetcherObserverPtr;
        //////////////////////////////////////////////////////////////////////////
        static bool s_prefetchScripts( pybind::kernel_interface * _kernel, const pybind::object & _cb, const pybind::args & _args )
        {
            PyPrefetcherObserverPtr observer = Helper::makeFactorableUnique<PyMutexPrefetcherObserver>( _kernel, _cb, _args );

            SCRIPT_SERVICE()
                ->prefetchModules( observer );

            uint32_t count = observer->getCount();

            return count != 0;
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_unfetchFonts()
        {
            TEXT_SERVICE()
                ->foreachFonts( []( const TextFontInterfacePtr & _font )
            {
                _font->unfetch();
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    ResourcePrefetcherPlugin::ResourcePrefetcherPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourcePrefetcherPlugin::~ResourcePrefetcherPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourcePrefetcherPlugin::_initializePlugin()
    {
        SERVICE_CREATE( ResourcePrefetcherService );

        if( SERVICE_EXIST( ScriptServiceInterface ) == true )
        {
            pybind::kernel_interface * kernel = SCRIPT_SERVICE()
                ->getKernel();

            pybind::set_kernel( kernel );

            pybind::def_function_args( kernel, "prefetchResources", &Detail::s_prefetchResources );
            pybind::def_function( kernel, "unfetchResources", &Detail::s_unfetchResources );
            pybind::def_function_args( kernel, "prefetchFonts", &Detail::s_prefetchFonts );
            pybind::def_function_kernel_args( kernel, "prefetchScripts", &Detail::s_prefetchScripts );
            pybind::def_function( kernel, "unfetchFonts", &Detail::s_unfetchFonts );
        }

        VOCABULARY_SET( ResourcePrefetcherInterface, STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "Default" ), Helper::makeFactorableUnique<DefaultResourcePrefetcher>() );
        VOCABULARY_SET( ResourcePrefetcherInterface, STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "Dataflow" ), Helper::makeFactorableUnique<DataflowResourcePrefetcher>() );

        ArchiveResourcePrefetcherPtr archivePrefetcherLZ4 = Helper::makeFactorableUnique<ArchiveResourcePrefetcher>();

        ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );

        if( archivator == nullptr )
        {
            return false;
        }

        archivePrefetcherLZ4->setArchivator( archivator );

        VOCABULARY_SET( ResourcePrefetcherInterface, STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "ArchiveLZ4" ), archivePrefetcherLZ4 );

        VOCABULARY_SET( ResourcePrefetcherInterface, STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "ImageDecoder" ), Helper::makeFactorableUnique<ImageDecoderResourcePrefetcher>() );
        VOCABULARY_SET( ResourcePrefetcherInterface, STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "SoundDecoder" ), Helper::makeFactorableUnique<SoundDecoderResourcePrefetcher>() );

        ResourcePrefetcherInterfacePtr prefetcherImageDecoder = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "ImageDecoder" ) );

        RESOURCEPREFETCHER_SERVICE()
            ->addResourcePrefetcher( STRINGIZE_STRING_LOCAL( "ResourceImageDefault" ), prefetcherImageDecoder );

        ResourcePrefetcherInterfacePtr prefetcherSoundDecoder = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "SoundDecoder" ) );

        RESOURCEPREFETCHER_SERVICE()
            ->addResourcePrefetcher( STRINGIZE_STRING_LOCAL( "ResourceSound" ), prefetcherSoundDecoder );

        ResourcePrefetcherInterfacePtr prefetcherDefault = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "Default" ) );

        RESOURCEPREFETCHER_SERVICE()
            ->addResourcePrefetcher( STRINGIZE_STRING_LOCAL( "ResourceHIT" ), prefetcherDefault );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourcePrefetcherPlugin::_finalizePlugin()
    {
        if( SERVICE_EXIST( ScriptServiceInterface ) == true )
        {
            pybind::kernel_interface * kernel = SCRIPT_SERVICE()
                ->getKernel();

            kernel->undef_adapter( "prefetchResources", nullptr );
            kernel->undef_adapter( "unfetchResources", nullptr );
        }

        RESOURCEPREFETCHER_SERVICE()
            ->removeResourcePrefetcher( STRINGIZE_STRING_LOCAL( "ResourceImageDefault" ) );

        RESOURCEPREFETCHER_SERVICE()
            ->removeResourcePrefetcher( STRINGIZE_STRING_LOCAL( "ResourceSound" ) );

        RESOURCEPREFETCHER_SERVICE()
            ->removeResourcePrefetcher( STRINGIZE_STRING_LOCAL( "ResourceHIT" ) );

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "Default" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "Dataflow" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "ArchiveLZ4" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "ImageDecoder" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "SoundDecoder" ) );

        SERVICE_FINALIZE( Mengine::ResourcePrefetcherServiceInterface );
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourcePrefetcherPlugin::_destroy()
    {
        SERVICE_DESTROY( Mengine::ResourcePrefetcherServiceInterface );
    }
}
