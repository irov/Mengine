#pragma once

#include "Interface/StreamInterface.h"
#include "Interface/FileSystemInterface.h"
#include "Interface/SoundCodecInterface.h"

#include "ThreadTaskPrefetch.h"

namespace Mengine
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
		SoundDecoderInterfacePtr m_soundDecoder;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ThreadTaskPrefetchSoundDecoder> ThreadTaskPrefetchSoundDecoderPtr;
}