#	pragma once

#	include "Kernel/ResourceImage.h"

#	include "Config/Typedef.h"

namespace Menge
{
	class ResourceImageData
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceImageData )

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
}