#	pragma once

#	include "Codec/VideoDecoder.h"

#	include "Core/InputStreamBuffer.h"

#	include "gvf.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
	class VideoDecoderGVF
		: public VideoDecoder
	{
	public:
		VideoDecoderGVF();
		~VideoDecoderGVF();

	public:
		bool _initialize() override;
		
    public:
		size_t decode( void * _buffer, size_t _bufferSize ) override;
	
	public:	
		float getTiming()  const override;
		bool seek( float _timing ) override;

    public:
        void setPitch( size_t _pitch ) override;

	public:		
		EVideoDecoderReadState readNextFrame( float & _pts ) override;

	protected:
		InputStreamBuffer2048 m_streamBuffer;

		gvf_decoder * m_gvf;
		size_t m_frame;
		size_t m_frames;
		float m_pts;		

		size_t m_pitch;
	};
}	// namespace Menge
