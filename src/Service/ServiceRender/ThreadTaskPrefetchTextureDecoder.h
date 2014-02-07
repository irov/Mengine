#	pragma once

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/StreamInterface.h"

#	include "Codec/ImageDecoderMemory.h"

#	include "Kernel/ThreadTask.h"

namespace Menge
{
	class ThreadTaskPrefetchTextureDecoder
		: public ThreadTask
	{
	public:
		ThreadTaskPrefetchTextureDecoder( ServiceProviderInterface * _serviceProvider, const ConstString& _pakName, const FilePath & _fileName, const ConstString & _codec, RenderTextureDecoderReceiverInterface * _receiver );
		
	protected:
		bool _onRun() override;

	protected:
		bool _onMain() override;

	protected:
		void _onComplete( bool _successful ) override;

	protected:
		void destroy() override;

	protected:
		ServiceProviderInterface * m_serviceProvider;

		ConstString m_pakName;
		FilePath m_fileName; 
		ConstString m_codec;

		RenderTextureDecoderReceiverInterface * m_receiver;

		InputStreamInterfacePtr m_stream;
		ImageDecoderInterfacePtr m_imageDecoder;

		ImageDecoderMemoryPtr m_imageDecoderMemory;
	};
}