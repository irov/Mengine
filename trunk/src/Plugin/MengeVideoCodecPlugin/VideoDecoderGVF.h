#	pragma once

#	include "Codec/VideoDecoder.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    struct gvf_header
    {
        size_t count;
        size_t fps;
        size_t width;
        size_t height;         
        size_t flag;
    };
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
		bool eof() override;
		//int sync( float _timing ) override;
		float getTiming()  const override;
		bool seek( float _timing ) override;

    public:
        void setPitch( size_t _pitch ) override;

	public:		
		EVideoDecoderReadState readNextFrame( float & _pts ) override;

	protected:
        gvf_header m_header;

        typedef std::vector<size_t> TVectorOffsets;
        TVectorOffsets m_offsets;

        size_t m_frame;
		float m_pts;

        size_t m_pitch;
	};
}	// namespace Menge
