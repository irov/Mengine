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
        const FilePath & getFileName() const;
        const ConstString & getCodecType() const;
        
    public:
        void setImagePath( const FilePath& _imagePath );

	protected:
		bool _isValid() const override;

	protected:
		bool _loader( const Metabuf::Metadata * _parser ) override;

	protected:
		bool _compile() override;
        		
	protected:
        FilePath m_fileName;
        ConstString m_codecType;                
	};
}