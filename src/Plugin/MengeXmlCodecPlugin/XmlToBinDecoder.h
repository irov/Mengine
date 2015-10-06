#	pragma once

#	include "Interface/XmlCodecInterface.h"
#   include "Interface/InputSystemInterface.h"
#   include "Interface/ArchiveInterface.h"

namespace Menge
{
	class XmlToBinDecoder
        : public XmlDecoderInterface
	{
	public:
		XmlToBinDecoder();
	
	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
		ServiceProviderInterface * getServiceProvider() const override;

    public:
        bool initialize() override;
		void finalize() override;

    protected:
        const InputStreamInterfacePtr & getStream() const override;

	public:
		bool setOptions( const CodecOptions * _options ) override;

	public:
		void setCodecDataInfo( const CodecDataInfo * _dataInfo ) override;
		const XmlCodecDataInfo * getCodecDataInfo() const override;

	public:
		bool prepareData( const InputStreamInterfacePtr & _stream ) override;
		
	public:		
		size_t decode( void * _buffer, size_t _bufferSize ) override;
		bool rewind() override;

	public:
		bool seek( float _timing ) override;
		float tell() const override;

	protected:
        ServiceProviderInterface * m_serviceProvider;

		ArchivatorInterfacePtr m_archivator;

        InputStreamInterfacePtr m_stream;

		XmlCodecOptions m_options;
	};
}