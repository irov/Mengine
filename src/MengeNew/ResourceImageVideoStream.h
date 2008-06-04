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
		ResourceImageVideoStream( const ResourceFactoryParam & _params );

	public:
		virtual unsigned int getCount() const override;
		virtual const mt::vec2f & getMaxSize( unsigned int _frame ) const override;
		virtual const mt::vec2f & getSize( unsigned int _frame ) const override;
		virtual const mt::vec2f & getOffset( unsigned int _frame ) const override;
		virtual const mt::vec4f & getUV( unsigned int _frame ) const override;		
		virtual const RenderImageInterface * getImage( unsigned int _frame ) const override;

		RenderImageInterface * getImage( unsigned int _frame );
		RenderVideoStreamInterface* getStream();

		void setFilePath( const std::string & _path );

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