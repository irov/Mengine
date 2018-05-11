#pragma once

#include "Kernel/ResourceImage.h"

#include "Config/Typedef.h"

namespace Mengine
{
	class ResourceImageDefault
		: public ResourceImage
	{
		DECLARE_VISITABLE( ResourceImage );

	public:
		ResourceImageDefault();
        
	public:
		void setup( const FilePath & _imagePath, const ConstString & _codecType, const mt::uv4f & _uv_image, const mt::uv4f & _uv_alpha, const mt::vec2f & _maxSize );

    public:
        const FilePath & getFilePath() const;
        const ConstString & getCodecType() const;
        
	protected:
		bool _isValid() const override;

	protected:
		bool _loader( const Metabuf::Metadata * _parser ) override;
		bool _convert() override;

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		void prepareImageFrame_();

	protected:
        FilePath m_filePath;
        ConstString m_codecType;
		ConstString m_converter;

		bool m_validNoExist;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<ResourceImageDefault> ResourceImageDefaultPtr;
}