#	pragma once

#	include "Interface/RenderSystemInterface.h"
#	include "math/vec4.h"

#	include "Core/ConstString.h"

namespace Menge
{
	class RenderTexture
		: public RenderTextureInterface
	{
	public:
		RenderTexture();
		~RenderTexture();

    public:
        void initialize( ServiceProviderInterface * _serviceProvider
            , const RenderImageInterfacePtr & _image
			, uint32_t _mipmaps
            , uint32_t _width
            , uint32_t _height
            , uint32_t _channels
            , uint32_t _id
			);

	public:
		void release() override;

	public:
		const RenderImageInterfacePtr & getImage() const override;
        
		uint32_t getId() const override;

		void setFileName( const FilePath & _fileName ) override;
		const FilePath & getFileName() const override;
		
		const Rect & getRect() const override;
		const Rect & getHWRect() const override;

		const mt::vec4f & getUV() const override;

		uint32_t getMipmaps() const override;

		uint32_t getWidth() const override;
		uint32_t getHeight() const override;

		uint32_t getChannels() const override;
		
		void * lock( size_t * _pitch, uint32_t _miplevel, const Rect & _rect, bool _readOnly = true ) const override;

		void unlock( uint32_t _miplevel ) const override;

		size_t getMemoryUse() const override;

	protected:
        ServiceProviderInterface * m_serviceProvider;

		RenderImageInterfacePtr m_image;

		FilePath m_fileName;
		
		uint32_t m_mipmaps;
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_channels;
		
		Rect m_rect;
        Rect m_hwRect;
		mt::vec4f m_uv;

		uint32_t m_id;
	};
}	// namespace Menge
