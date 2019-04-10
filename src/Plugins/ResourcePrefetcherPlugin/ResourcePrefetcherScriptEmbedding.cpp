#include "ResourcePrefetcherScriptEmbedding.h"

#include "Interface/ScriptServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/TextServiceInterface.h"

#include "ResourcePrefetcherServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"

#include "pybind/pybind.hpp"

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
            PyPrefetcherObserver( const ConstString & _category, const ConstString & _groupName, const pybind::object & _cb, const pybind::args & _args )
                : m_category( _category )
                , m_groupName( _groupName )
                , m_cb( _cb )
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
                    m_cb.call_args( m_successful, m_args );
                    m_cb.reset();
                    m_args.reset();
                }
            }

            void onPrefetchComplete( bool _successful ) override
            {
                m_successful &= _successful;

                if( --m_count == 0 )
                {
                    m_cb.call_args( m_successful, m_args );
                    m_cb.reset();
                    m_args.reset();
                }
            }

        protected:
            ConstString m_category;
            ConstString m_groupName;

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
            LOGGER_STATISTIC( "prefetch resources category '%s' group '%s'"
                , _category.c_str()
                , _groupName.c_str()
            );

            const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
                ->getFileGroup( _category );

            if( fileGroup == nullptr )
            {
                return false;
            }

            PyPrefetcherObserverPtr observer = Helper::makeFactorableUnique<PyPrefetcherObserver>( _category, _groupName, _cb, _args );

            RESOURCE_SERVICE()
                ->foreachGroupResources( fileGroup, _groupName, [&observer]( const ResourcePtr & _resource )
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
            LOGGER_STATISTIC( "unfetch resources category '%s' group '%s'"
                , _category.c_str()
                , _groupName.c_str()
            );

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
            PyPrefetcherObserverPtr observer = Helper::makeFactorableUnique<PyPrefetcherObserver>( ConstString::none(), ConstString::none(), _cb, _args );

            TEXT_SERVICE()
                ->foreachFonts( [&observer]( const TextFontInterfacePtr & _textFont )
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
            PyMutexPrefetcherObserver( pybind::kernel_interface * _kernel, const ConstString & _category, const ConstString & _groupName, const pybind::object & _cb, const pybind::args & _args )
                : PyPrefetcherObserver( _category, _groupName, _cb, _args )
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
            PyPrefetcherObserverPtr observer = Helper::makeFactorableUnique<PyMutexPrefetcherObserver>( _kernel, ConstString::none(), ConstString::none(), _cb, _args );

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
    ResourcePrefetcherScriptEmbedding::ResourcePrefetcherScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourcePrefetcherScriptEmbedding::~ResourcePrefetcherScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourcePrefetcherScriptEmbedding::embedding( pybind::kernel_interface * _kernel )
    {
        pybind::def_function_args( _kernel, "prefetchResources", &Detail::s_prefetchResources );
        pybind::def_function( _kernel, "unfetchResources", &Detail::s_unfetchResources );
        pybind::def_function_args( _kernel, "prefetchFonts", &Detail::s_prefetchFonts );
        pybind::def_function_kernel_args( _kernel, "prefetchScripts", &Detail::s_prefetchScripts );
        pybind::def_function( _kernel, "unfetchFonts", &Detail::s_unfetchFonts );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourcePrefetcherScriptEmbedding::ejecting( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "prefetchResources", nullptr );
        _kernel->remove_from_module( "unfetchResources", nullptr );
        _kernel->remove_from_module( "prefetchFonts", nullptr );
        _kernel->remove_from_module( "prefetchScripts", nullptr );
        _kernel->remove_from_module( "unfetchFonts", nullptr );
    }
}

