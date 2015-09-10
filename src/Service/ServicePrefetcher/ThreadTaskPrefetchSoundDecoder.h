#	pragma once

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/StreamInterface.h"
#	include "Interface/FileSystemInterface.h"
#	include "Interface/SoundCodecInterface.h"

#	include "ThreadTaskPrefetch.h"

namespace Menge
{
	class ThreadTaskPrefetchSoundDecoder
		: public ThreadTaskPrefetch
	{
	public:
		ThreadTaskPrefetchSoundDecoder();
	
	public:
		void setSoundCodec( const ConstString & _codec );
		
	public:
		const SoundDecoderInterfacePtr & getDecoder() const;

	protected:
		bool _onRun() override;
		bool _onMain() override;

	protected:
		ConstString m_codec;
				
	protected:
		FileGroupInterfacePtr m_group;
		InputStreamInterfacePtr m_stream;
		SoundDecoderInterfacePtr m_soundDecoder;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ThreadTaskPrefetchSoundDecoder> ThreadTaskPrefetchSoundDecoderPtr;
}