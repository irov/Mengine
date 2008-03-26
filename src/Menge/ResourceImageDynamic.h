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
		ResourceImageDynamic( const std::string & _name );

	public:
		virtual unsigned int getCount() const override;
		virtual const mt::vec2f & getMaxSize( unsigned int _frame ) const override;
		virtual const mt::vec2f & getSize( unsigned int _frame ) const override;
		virtual const mt::vec2f & getOffset( unsigned int _frame ) const override;
		virtual const mt::vec4f & getUV( unsigned int _frame ) const override;		
		virtual const RenderImageInterface * getImage( unsigned int _frame ) const override;

		RenderImageInterface * getImage( unsigned int _frame );

	public:
		void loader( XmlElement * _xml ) override;

	public:
		void setRenderImage( RenderImageInterface* _image );

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		ImageFrame m_frame;
		mt::vec2f m_offset;
		mt::vec4f m_uv;
	};
}