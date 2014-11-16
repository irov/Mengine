#	pragma once

#	include "Codec/VideoDecoder.h"

#   include "Interface/ArchiveInterface.h"

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
		void _finalize() override;

	public:
		bool _prepareData() override;
		
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
		ArchivatorInterfacePtr m_archivator;

		uint32_t m_frame;
		uint32_t m_frameCount;

		float m_pts;

		size_t * m_framesOffset;
	};
}	// namespace Menge
