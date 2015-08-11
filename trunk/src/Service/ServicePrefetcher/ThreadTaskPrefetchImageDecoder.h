#	pragma once

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/StreamInterface.h"
#	include "Interface/FileSystemInterface.h"
#	include "Interface/ImageCodecInterface.h"

#	include "Kernel/ThreadTask.h"

#	include "Core/MemoryInput.h"

namespace Menge
{
	class ThreadTaskPrefetchImageDecoder
		: public ThreadTask
	{
	public:
		ThreadTaskPrefetchImageDecoder();
	
	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider );

	public:
		void initialize( const ConstString& _pakName, const FilePath & _fileName, const ConstString & _codec );
		
	public:
		inline const ConstString & getPakName() const;
		inline const FilePath & getFilePath() const;

		const ImageDecoderInterfacePtr & getDecoder() const;

	protected:
		bool _onRun() override;
		bool _onMain() override;
		void _onComplete( bool _successful ) override;

	protected:
		ServiceProviderInterface * m_serviceProvider;

		ConstString m_pakName;
		FilePath m_filePath; 
		ConstString m_codec;
				
	protected:
		FileGroupInterfacePtr m_group;
		InputStreamInterfacePtr m_stream;
		ImageDecoderInterfacePtr m_imageDecoder;
	};
	//////////////////////////////////////////////////////////////////////////
	inline const ConstString & ThreadTaskPrefetchImageDecoder::getPakName() const
	{
		return m_pakName;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const FilePath & ThreadTaskPrefetchImageDecoder::getFilePath() const
	{
		return m_filePath;
	}
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ThreadTaskPrefetchImageDecoder> ThreadTaskPrefetchImageDecoderPtr;
}