#	pragma once

#	include "ResourceImage.h"

class RenderImageInterface;

namespace Menge
{
	class ResourceImageDynamic
		: public ResourceImage
	{
		RESOURCE_DECLARE( ResourceImageDynamic )

	public:
		ResourceImageDynamic();
		~ResourceImageDynamic();

	public:
		void setSize( const mt::vec2f& _size );

	public:
		std::size_t getCount() const override;
		const mt::vec2f & getMaxSize( std::size_t _frame ) const override;
		const mt::vec2f & getSize( std::size_t _frame ) const override;
		const mt::vec2f & getOffset( std::size_t _frame ) const override;
		const mt::vec4f & getUV( std::size_t _frame ) const override;		
		bool isAlpha( std::size_t _frame ) const override;

		Texture* getTexture( std::size_t _frame ) override;

		const ConstString & getFilename( std::size_t _frame ) const override;
		const ConstString & getCodecType( std::size_t _frame ) const override;
		std::size_t getFilenameCount() const override;

	public:
		void loader( BinParser * _parser ) override;

	public:
		void setRenderImage( Texture* _image );

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		ImageFrame m_frame;
		mt::vec2f m_offset;
		mt::vec4f m_uv;

		ConstString m_cachedPath;
		ConstString m_cachedCodec;
	};
}
