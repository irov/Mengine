#	pragma once

#	include "Interface/PickCodecInterface.h"

namespace Menge
{
	class PickEncoder
		: public PickEncoderInterface
	{
	public:
		PickEncoder();
        
    public:
        bool initialize( ServiceProviderInterface * _serviceProvider, const OutputStreamInterfacePtr & _stream ) override;

    protected:
        virtual bool _initialize();

    public:
        bool setOptions( CodecOptions * _options ) override;

    protected:
        virtual bool _invalidateOptions();

    public:
        OutputStreamInterfacePtr getStream() const override;
        
	protected:
        ServiceProviderInterface * m_serviceProvider;
        OutputStreamInterfacePtr m_stream;

		PickCodecOptions m_options;
	};
}
