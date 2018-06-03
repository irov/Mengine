#pragma once

#include "Kernel/Resource.h"

#include "Config/Typedef.h"

#include "math/vec2.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	class ResourceImageData
		: public Resource
	{
		DECLARE_VISITABLE( Resource );

	public:
		ResourceImageData();
        ~ResourceImageData() override;
        
    public:
        const FilePath & getFileName() const;
        const ConstString & getCodecType() const;
        
    public:
        void setImagePath( const FilePath & _imagePath );
		const FilePath & getImagePath() const;

	public:
		const mt::vec2f & getImageMaxSize() const;
        uint8_t * getImageBuffer() const;

	protected:
		bool _isValid() const override;

	protected:
		bool _loader( const Metabuf::Metadata * _parser ) override;

	protected:
		bool _compile() override;
		void _release() override;

	protected:
        FilePath m_fileName;
        ConstString m_codecType;

		mt::vec2f m_maxSize;
		uint8_t * m_buffer;

		bool m_validNoExist;		
	};
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<ResourceImageData> ResourceImageDataPtr;
}