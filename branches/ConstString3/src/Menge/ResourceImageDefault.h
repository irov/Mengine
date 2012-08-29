#	pragma once

#	include "ResourceImage.h"

#	include "Config/Typedef.h"

#	include <vector>

namespace Menge
{
	class ResourceImageDefault
		: public ResourceImage
	{
		RESOURCE_DECLARE( ResourceImageDefault )

	public:
		ResourceImageDefault();
        
    public:
        const WString & getFileName() const;
        const ConstString & getCodecType() const;
        
    protected:
		bool loadBufferAlpha( unsigned char * _buffer, int _pitch ) override;

    public:
        void setImagePath( const WString& _imagePath );

	protected:
		bool isValid() const override;

	protected:
		void loader( const Metabuf::Metadata * _parser ) override;

	protected:
		bool _compile() override;
		void _release() override;
        		
	protected:
        WString m_fileName;
        ConstString m_codecType;                
	};
}