#	pragma once

#	include "Interface/PickCodecInterface.h"

#	include "Codec/Encoder.h"

namespace Menge
{
	class PickEncoder
		: public Encoder<PickEncoderInterface>
	{
	public:
		PickEncoder();
        
    public:
        bool setOptions( CodecOptions * _options ) override;

    protected:
        virtual bool _invalidateOptions();
       
	protected:
		PickCodecOptions m_options;
	};
}
