#	pragma once

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/StreamInterface.h"
#	include "Interface/FileSystemInterface.h"
#	include "Interface/SoundCodecInterface.h"

#	include "Kernel/ThreadTask.h"

#	include "Core/MemoryInput.h"

namespace Menge
{
	class ThreadTaskPrefetchSoundDecoder
		: public ThreadTask
	{
	public:
		ThreadTaskPrefetchSoundDecoder();
	
	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider );

	public:
		void initialize( const ConstString& _pakName, const FilePath & _fileName, const ConstString & _codec );
		
	public:
		const SoundDecoderInterfacePtr & getDecoder() const;

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
		SoundDecoderInterfacePtr m_soundDecoder;
		MemoryInputPtr m_memoryInput;
	};

	typedef stdex::intrusive_ptr<ThreadTaskPrefetchSoundDecoder> ThreadTaskPrefetchSoundDecoderPtr;
}