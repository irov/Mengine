#	pragma once

#	include "Kernel/ResourceImage.h"

#	include "Config/Typedef.h"

namespace Menge
{
	class ResourceImageDefault
		: public ResourceImage
	{
		RESOURCE_DECLARE( ResourceImageDefault )

	public:
		ResourceImageDefault();
        
    public:
        const FilePath & getFilePath() const;
        const ConstString & getCodecType() const;
        
    public:
        void setImagePath( const FilePath & _imagePath );
		const FilePath & getImagePath() const;

	protected:
		bool _isValid() const override;

	protected:
		bool _loader( const Metabuf::Metadata * _parser ) override;

	protected:
		bool _compile() override;

	protected:
        FilePath m_filePath;
        ConstString m_codecType;

		bool m_validNoExist;
	};
}