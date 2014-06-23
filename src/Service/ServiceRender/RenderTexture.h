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
            , size_t _width
            , size_t _height
            , size_t _channels
            , size_t _id
			);

	public:
		void release() override;

	public:
		const RenderImageInterfacePtr & getImage() const override;
        
		size_t getId() const override;

		void setFileName( const FilePath & _fileName ) override;
		const FilePath & getFileName() const override;
		
		const Rect & getRect() const override;
		const Rect & getHWRect() const override;

		const mt::vec4f & getUV() const override;

		size_t getWidth() const override;
		size_t getHeight() const override;

		size_t getChannels() const override;
		
		unsigned char* lock( int* _pitch, const Rect& _rect, bool _readOnly = true ) const override;

		void unlock() const override;

		size_t getMemoryUse() const override;

	protected:
        ServiceProviderInterface * m_serviceProvider;

		RenderImageInterfacePtr m_image;

		FilePath m_fileName;
		
		size_t m_width;
		size_t m_height;
		size_t m_channels;
		
		Rect m_rect;
        Rect m_hwRect;
		mt::vec4f m_uv;

		size_t m_id;
	};
}	// namespace Menge
