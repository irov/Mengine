//#	pragma once
//
//#	include "Codec/VideoDecoder.h"
//
//namespace Menge
//{
//    //////////////////////////////////////////////////////////////////////////
//	class VideoDecoderGVFPrefetcher
//	{
//	public:
//		VideoDecoderGVFPrefetcher();
//		~VideoDecoderGVFPrefetcher();
//
//    public:
//        void initialize( ServiceProviderInterface * _serviceProvider );
//        void finalize();
//
//    public:
//        void prefetchFrame( const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, size_t _frame );
//
//	protected:
//        ServiceProviderInterface * m_serviceProvider;
//
//        ThreadJob m_prefetcherJob;
//
//        TBlobject m_buffer;
//        
//        size_t m_frame;
//	};
//}	// namespace Menge
