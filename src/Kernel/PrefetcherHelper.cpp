#include "PrefetcherHelper.h"

#include "Interface/PrefetcherServiceInterface.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool prefetchImageDecoder( const ContentInterfacePtr & _content, const PrefetcherObserverInterfacePtr & _observer )
        {
            if( SERVICE_IS_INITIALIZE( PrefetcherServiceInterface ) == false )
            {
                _observer->onPrefetchIgnored();

                return false;
            }

            if( PREFETCHER_SERVICE()
                ->prefetchImageDecoder( _content, _observer ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getPrefetchImageDecoder( const ContentInterfacePtr & _content, ImageDecoderInterfacePtr * const _decoder )
        {
            if( SERVICE_IS_INITIALIZE( PrefetcherServiceInterface ) == false )
            {
                return false;
            }

            if( PREFETCHER_SERVICE()
                ->getImageDecoder( _content, _decoder ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool prefetchSoundDecoder( const ContentInterfacePtr & _content, const PrefetcherObserverInterfacePtr & _observer )
        {
            if( SERVICE_IS_INITIALIZE( PrefetcherServiceInterface ) == false )
            {
                _observer->onPrefetchIgnored();

                return false;
            }

            if( PREFETCHER_SERVICE()
                ->prefetchSoundDecoder( _content, _observer ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getPrefetchSoundDecoder( const ContentInterfacePtr & _content, SoundDecoderInterfacePtr * const _decoder )
        {
            if( SERVICE_IS_INITIALIZE( PrefetcherServiceInterface ) == false )
            {
                return false;
            }

            if( PREFETCHER_SERVICE()
                ->getSoundDecoder( _content, _decoder ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool prefetchData( const ContentInterfacePtr & _content, const DataflowContext * _context, const PrefetcherObserverInterfacePtr & _observer )
        {
            if( SERVICE_IS_INITIALIZE( PrefetcherServiceInterface ) == false )
            {
                _observer->onPrefetchIgnored();

                return false;
            }

            if( PREFETCHER_SERVICE()
                ->prefetchData( _content, _context, _observer ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getPrefetchData( const ContentInterfacePtr & _content, DataInterfacePtr * const _data )
        {
            if( SERVICE_IS_INITIALIZE( PrefetcherServiceInterface ) == false )
            {
                return false;
            }

            if( PREFETCHER_SERVICE()
                ->getData( _content, _data ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool popPrefetchData( const ContentInterfacePtr & _content, DataInterfacePtr * const _data )
        {
            if( SERVICE_IS_INITIALIZE( PrefetcherServiceInterface ) == false )
            {
                return false;
            }

            if( PREFETCHER_SERVICE()
                ->popData( _content, _data ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool prefetchStream( const ContentInterfacePtr & _content, const ArchivatorInterfacePtr & _archivator, uint32_t _magicNumber, uint32_t _magicVersion, const PrefetcherObserverInterfacePtr & _observer )
        {
            if( SERVICE_IS_INITIALIZE( PrefetcherServiceInterface ) == false )
            {
                _observer->onPrefetchIgnored();

                return false;
            }

            if( PREFETCHER_SERVICE()
                ->prefetchStream( _content, _archivator, _magicNumber, _magicVersion, _observer ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getPrefetchStream( const ContentInterfacePtr & _content, MemoryInterfacePtr * const _memory )
        {
            if( SERVICE_IS_INITIALIZE( PrefetcherServiceInterface ) == false )
            {
                return false;
            }

            if( PREFETCHER_SERVICE()
                ->getStream( _content, _memory ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool unfetch( const ContentInterfacePtr & _content )
        {
            if( SERVICE_IS_INITIALIZE( PrefetcherServiceInterface ) == false )
            {
                return false;
            }

            if( PREFETCHER_SERVICE()
                ->unfetch( _content ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void foreachPrefetches( const LambdaPrefetchTask & _lambda )
        {
            if( SERVICE_IS_INITIALIZE( PrefetcherServiceInterface ) == false )
            {
                return;
            }

            PREFETCHER_SERVICE()
                ->foreachPrefetches( _lambda );
        }
        //////////////////////////////////////////////////////////////////////////
        void visitPrefetches( Visitor * _visitor )
        {
            if( SERVICE_IS_INITIALIZE( PrefetcherServiceInterface ) == false )
            {
                return;
            }

            PREFETCHER_SERVICE()
                ->visitPrefetches( _visitor );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}