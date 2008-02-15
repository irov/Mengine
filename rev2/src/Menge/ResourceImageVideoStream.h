#	pragma once

#	include "ResourceImage.h"

class RenderVideoStreamInterface;

namespace Menge
{
	class ResourceImageVideoStream
		: public ResourceImage
	{
		RESOURCE_DECLARE( ResourceImageVideoStream )

	public:
		ResourceImageVideoStream( const std::string & _name );

	public:
		virtual size_t getCount() const override;
		virtual const mt::vec2f & getMaxSize( size_t _frame ) const override;
		virtual const mt::vec2f & getSize( size_t _frame ) const override;
		virtual const mt::vec2f & getOffset( size_t _frame ) const override;
		virtual const mt::vec4f & getUV( size_t _frame ) const override;		
		virtual const RenderImageInterface * getImage( size_t _frame ) const override;

		RenderImageInterface * getImage( size_t _frame );
		RenderVideoStreamInterface* getStream();

	public:
		void loader( XmlElement * _xml ) override;

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		RenderVideoStreamInterface* m_interface;
		std::string m_filename;
		mt::vec2f m_offset;
		mt::vec4f m_uv;
		mt::vec2f m_size;
	};
}