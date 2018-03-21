#pragma once

#include "Kernel/ResourceReference.h"

#include "Config/Typedef.h"

#   include "math/vec2.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	class ResourceImageData
		: public ResourceReference
	{
		DECLARE_VISITABLE( ResourceReference );

	public:
		ResourceImageData();
        
    public:
        const FilePath & getFileName() const;
        const ConstString & getCodecType() const;
        
    public:
        void setImagePath( const FilePath & _imagePath );
		const FilePath & getImagePath() const;

	public:
		const mt::vec2f & getImageMaxSize() const;
		unsigned char * getImageBuffer() const;

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
		unsigned char * m_buffer;

		bool m_validNoExist;		
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ResourceImageData> ResourceImageDataPtr;
}