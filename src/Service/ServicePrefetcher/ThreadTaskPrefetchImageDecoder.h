#	pragma once

#	include "Interface/StreamInterface.h"
#	include "Interface/FileSystemInterface.h"
#	include "Interface/ImageCodecInterface.h"

#	include "ThreadTaskPrefetch.h"

namespace Menge
{
	class ThreadTaskPrefetchImageDecoder
		: public ThreadTaskPrefetch
	{
	public:
		ThreadTaskPrefetchImageDecoder();

	public:
		void setImageCodec( const ConstString & _codec );
		
	public:
		const ImageDecoderInterfacePtr & getDecoder() const;

	protected:
		bool _onRun() override;
		bool _onMain() override;

	protected:
		ConstString m_codec;
				
	protected:
		ImageDecoderInterfacePtr m_imageDecoder;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ThreadTaskPrefetchImageDecoder> ThreadTaskPrefetchImageDecoderPtr;
}