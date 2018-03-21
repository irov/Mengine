//#	include "VideoDecoderGVFPrefetcher.h"
//
//namespace Menge
//{
//    ////////////////////////////////////////////////////////////////////////
//    class VideoDecoderGVFWorker
//        : public ThreadWorkerInterface
//    {
//    public:        
//        void initialize( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, size_t _frame );
//
//    public:
//        bool onWork( size_t _id ) override;
//        void onDone( size_t _id ) override;
//
//    protected:
//        TBlobject m_buffer;
//    };
//    ////////////////////////////////////////////////////////////////////////
//    VideoDecoderGVFPrefetcher::VideoDecoderGVFPrefetcher()
//        : m_serviceProvider(nullptr)
//        , m_frame(0)
//    {
//    }
//    ////////////////////////////////////////////////////////////////////////
//    VideoDecoderGVFPrefetcher::~VideoDecoderGVFPrefetcher()
//    {
//    }
//    ////////////////////////////////////////////////////////////////////////
//    void VideoDecoderGVFPrefetcher::initialize( ServiceProviderInterface * _serviceProvider )
//    {
//        m_serviceProvider = _serviceProvider;
//
//        THREAD_SERVICE(m_serviceProvider)
//            ->addTask( &m_prefetcherJob, 0 ); 
//    }
//    ////////////////////////////////////////////////////////////////////////
//    void VideoDecoderGVFPrefetcher::finalize()
//    {
//        THREAD_SERVICE(m_serviceProvider)
//            ->cancelTask( &m_threadGVFPrefetcher );
//    }
//    ////////////////////////////////////////////////////////////////////////
//    void VideoDecoderGVFPrefetcher::prefetchFrame( const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, size_t _frame )
//    {
//        m_buffer.resize( _size );
//
//	    _stream->seek( _offset );
//        _stream->read( &m_buffer[0], _size );
//
//        m_frame = _frame;
//
//
//        THREAD_SERVICE(m_serviceProvider)
//            ->
//    }
//}	// namespace Menge
