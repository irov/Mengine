#	pragma once

#	include "ResourceImpl.h"

#	include "Math/vec4.h"
#	include <vector>

class RenderImageInterface;

namespace Menge
{
	class ResourceImage
		: public ResourceImpl
	{
	public:
		ResourceImage( const std::string & _name );

		struct ImageFrame
		{
			mt::vec2f size;
			RenderImageInterface * image;
		};

	public:
		virtual size_t getCount() = 0;

		virtual const mt::vec2f & getMaxSize( size_t _frame ) const = 0;
		virtual const mt::vec2f & getSize( size_t _frame ) const = 0;
		virtual const mt::vec2f & getOffset( size_t _frame ) const = 0;
		virtual const mt::vec4f & getUV( size_t _frame ) const = 0;		

		virtual const RenderImageInterface * getImage( size_t _frame ) const = 0;

	public:
		void loader( TiXmlElement * _xml ) override;

	protected:
		ImageFrame loadImageFrame( const std::string & _fileName, std::vector<char> & _buff );
		void releaseImageFrame( const ImageFrame & _frame );

	protected:
		size_t m_filter;
	};
}