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
		ResourceImageDynamic( const ResourceFactoryParam & _params );
		~ResourceImageDynamic();

	public:
		virtual std::size_t getCount() const override;
		virtual const mt::vec2f & getMaxSize( std::size_t _frame ) const override;
		virtual const mt::vec2f & getSize( std::size_t _frame ) const override;
		virtual const mt::vec2f & getOffset( std::size_t _frame ) const override;
		virtual const mt::vec4f & getUV( std::size_t _frame ) const override;		
		virtual bool isAlpha( std::size_t _frame ) const override;

		Texture* getImage( std::size_t _frame );
		void setSize( const mt::vec2f& _size );

		virtual const String & getFilename( std::size_t _frame ) const;
		virtual std::size_t getFilenameCount() const override;

	public:
		void loader( XmlElement * _xml ) override;

	public:
		void setRenderImage( Texture* _image );

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		ImageFrame m_frame;
		mt::vec2f m_offset;
		mt::vec4f m_uv;
		bool m_cached;
	};
}
