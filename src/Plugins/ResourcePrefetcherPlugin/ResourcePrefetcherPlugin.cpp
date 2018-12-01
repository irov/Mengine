#include "ResourcePrefetcherPlugin.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"

#include "ResourcePrefetcherServiceInterface.h"

#include "ResourceImageDefaultPrefetcher.h"
#include "ResourceSoundPrefetcher.h"
#include "ResourceDefaultPrefetcher.h"

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

            void onPrefetchAlreadyExist()
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

            PyPrefetcherObserverPtr observer = new FactorableUnique<PyPrefetcherObserver>( _cb, _args );

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
    bool ResourcePrefetcherPlugin::_avaliable()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourcePrefetcherPlugin::_initialize()
    {
        SERVICE_CREATE( ResourcePrefetcherService );

        if( SERVICE_EXIST( ScriptServiceInterface ) == true )
        {
            pybind::kernel_interface * kernel = SCRIPT_SERVICE()
                ->getKernel();

            pybind::set_kernel( kernel );

            pybind::def_function_args( kernel, "prefetchResources", &Detail::s_prefetchResources );
            pybind::def_function( kernel, "unfetchResources", &Detail::s_unfetchResources );
        }

        RESOURCEPREFETCHER_SERVICE()
            ->addResourcePrefetcher( STRINGIZE_STRING_LOCAL( "ResourceImageDefault" ), new FactorableUnique<ResourceImageDefaultPrefetcher>() );

        RESOURCEPREFETCHER_SERVICE()
            ->addResourcePrefetcher( STRINGIZE_STRING_LOCAL( "ResourceSound" ), new FactorableUnique<ResourceSoundPrefetcher>() );

        RESOURCEPREFETCHER_SERVICE()
            ->addResourcePrefetcher( STRINGIZE_STRING_LOCAL( "ResourceHIT" ), new FactorableUnique<ResourceDefaultPrefetcher>() );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourcePrefetcherPlugin::_finalize()
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

        SERVICE_FINALIZE( Mengine::ResourcePrefetcherServiceInterface );
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourcePrefetcherPlugin::_destroy()
    {
        SERVICE_DESTROY( Mengine::ResourcePrefetcherServiceInterface );
    }
}
