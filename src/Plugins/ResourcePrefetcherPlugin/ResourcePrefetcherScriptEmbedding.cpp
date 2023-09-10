#include "ResourcePrefetcherScriptEmbedding.h"

#include "Interface/ScriptServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/TextServiceInterface.h"
#include "Interface/FontServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonDocumentTraceback.h"

#include "ResourcePrefetcherServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/FactorableUnique.h"

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
            PyPrefetcherObserver( const ConstString & _groupName, const pybind::object & _cb, const pybind::args & _args )
                : m_groupName( _groupName )
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
            ConstString m_groupName;

            pybind::object m_cb;
            pybind::args m_args;

            uint32_t m_count;
            bool m_successful;
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<PyPrefetcherObserver> PyPrefetcherObserverPtr;
        //////////////////////////////////////////////////////////////////////////
        static bool s_prefetchResources( const ConstString & _groupName, const pybind::object & _cb, const pybind::args & _args )
        {
            LOGGER_STATISTIC( "prefetch resources group '%s'"
                , _groupName.c_str()
            );

            PyPrefetcherObserverPtr observer = Helper::makeFactorableUnique<PyPrefetcherObserver>( MENGINE_DOCUMENT_PYBIND, _groupName, _cb, _args );

            RESOURCE_SERVICE()
                ->foreachGroupResources( _groupName, [&observer]( const ResourcePtr & _resource )
            {
                RESOURCEPREFETCHER_SERVICE()
                    ->prefetchResource( _resource, observer );
            } );

            uint32_t count = observer->getCount();

            return count != 0;
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_unfetchResources( const ConstString & _groupName )
        {
            LOGGER_STATISTIC( "unfetch resources category group '%s'"
                , _groupName.c_str()
            );

            RESOURCE_SERVICE()
                ->foreachGroupResources( _groupName, []( const ResourcePtr & _resource )
            {
                RESOURCEPREFETCHER_SERVICE()
                    ->unfetchResource( _resource );
            } );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_prefetchFonts( const pybind::object & _cb, const pybind::args & _args )
        {
            PyPrefetcherObserverPtr observer = Helper::makeFactorableUnique<PyPrefetcherObserver>( MENGINE_DOCUMENT_PYBIND, ConstString::none(), _cb, _args );

            FONT_SERVICE()
                ->foreachGlyphs( [&observer]( const FontGlyphInterfacePtr & _glyph )
            {
                _glyph->prefetch( observer );
            } );

            FONT_SERVICE()
                ->foreachFonts( [&observer]( const FontInterfacePtr & _font )
            {
                _font->prefetch( observer );
            } );

            uint32_t count = observer->getCount();

            return count != 0;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_prefetchScripts( const pybind::object & _cb, const pybind::args & _args )
        {
            PyPrefetcherObserverPtr observer = Helper::makeFactorableUnique<PyPrefetcherObserver>( MENGINE_DOCUMENT_PYBIND, ConstString::none(), _cb, _args );

            SCRIPT_SERVICE()
                ->prefetchModules( observer );

            uint32_t count = observer->getCount();

            return count != 0;
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_unfetchFonts()
        {
            FONT_SERVICE()
                ->foreachGlyphs( []( const FontGlyphInterfacePtr & _glyph )
            {
                _glyph->unfetch();
            } );

            FONT_SERVICE()
                ->foreachFonts( []( const FontInterfacePtr & _font )
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
    bool ResourcePrefetcherScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        SCRIPT_SERVICE()
            ->setAvailablePlugin( "ResourcePrefetcher", true );

        pybind::def_function_args( _kernel, "prefetchResources", &Detail::s_prefetchResources );
        pybind::def_function( _kernel, "unfetchResources", &Detail::s_unfetchResources );
        pybind::def_function_args( _kernel, "prefetchFonts", &Detail::s_prefetchFonts );
        pybind::def_function_args( _kernel, "prefetchScripts", &Detail::s_prefetchScripts );
        pybind::def_function( _kernel, "unfetchFonts", &Detail::s_unfetchFonts );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourcePrefetcherScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "prefetchResources", nullptr );
        _kernel->remove_from_module( "unfetchResources", nullptr );
        _kernel->remove_from_module( "prefetchFonts", nullptr );
        _kernel->remove_from_module( "prefetchScripts", nullptr );
        _kernel->remove_from_module( "unfetchFonts", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}

