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
		size_t getCount() const override;
		const mt::vec2f & getMaxSize( size_t _frame ) const override;
		const mt::vec2f & getSize( size_t _frame ) const override;
		const mt::vec2f & getOffset( size_t _frame ) const override;
		const mt::vec4f & getUV( size_t _frame ) const override;
		const mt::vec4f & getUVImage( size_t _frame ) const override;

		bool isAlpha( size_t _frame ) const override;
		Texture* getTexture( size_t _frame ) const override;

		const ConstString & getFileName( size_t _frame ) const override;
		const ConstString & getCodecType( size_t _frame ) const override;
		size_t getFilenameCount() const override;

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
