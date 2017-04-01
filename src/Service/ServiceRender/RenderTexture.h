#	pragma once

#	include "Interface/RenderSystemInterface.h"
#	include "math/vec4.h"

#   include "Core/ServantBase.h"
#	include "Core/ConstString.h"

namespace Menge
{
	class RenderTexture
		: public ServantBase<RenderTextureInterface>
	{
	public:
		RenderTexture();
		~RenderTexture();

    public:
        void initialize( uint32_t _id, const RenderImageInterfacePtr & _image, uint32_t _width, uint32_t _height );

	public:
		void release() override;

	public:
		const RenderImageInterfacePtr & getImage() const override;
   
	public:
		uint32_t getId() const override;

    public:
		void setCategory( const ConstString & _category ) override;
		const ConstString & getCategory() const override;

		void setFileName( const FilePath & _fileName ) override;
		const FilePath & getFileName() const override;
		
		const Rect & getRect() const override;

		const mt::uv4f & getUV() const override;

		uint32_t getWidth() const override;
		uint32_t getHeight() const override;

	protected:        
        uint32_t m_id;

		RenderImageInterfacePtr m_image;

		ConstString m_category;
		FilePath m_fileName;
		
		uint32_t m_width;
		uint32_t m_height;
		
		Rect m_rect;
		mt::uv4f m_uv;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<RenderTexture> RenderTexturePtr;
	//////////////////////////////////////////////////////////////////////////
	inline const ConstString & RenderTexture::getCategory() const
	{
		return m_category;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const FilePath & RenderTexture::getFileName() const
	{
		return m_fileName;
	}
}	// namespace Menge
