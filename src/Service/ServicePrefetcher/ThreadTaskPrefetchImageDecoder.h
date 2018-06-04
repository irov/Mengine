#pragma once

#include "Interface/StreamInterface.h"
#include "Interface/FileSystemInterface.h"
#include "Interface/ImageCodecInterface.h"

#include "ThreadTaskPrefetch.h"

namespace Mengine
{
	class ThreadTaskPrefetchImageDecoder
		: public ThreadTaskPrefetch
	{
	public:
		ThreadTaskPrefetchImageDecoder();
        ~ThreadTaskPrefetchImageDecoder() override;

	public:
		void setImageCodec( const ConstString & _codec );
        const ConstString & getImageCodec() const;

		
	public:
		const ImageDecoderInterfacePtr & getDecoder() const;

	protected:
		bool _onRun() override;
		bool _onMain() override;

	protected:
		ConstString m_imageCodec;
				
	protected:
		ImageDecoderInterfacePtr m_imageDecoder;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<ThreadTaskPrefetchImageDecoder> ThreadTaskPrefetchImageDecoderPtr;
}