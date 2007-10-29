#	pragma once

#	include "ResourceReference.h"

#	include "Math/vec4.h"
#	include <vector>

class RenderFontInterface;

namespace Menge
{
	class ResourceFont
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceFont )

	public:
		ResourceFont( const std::string & _name );

	public:
		float getCharWidth( char _id ) const;
		float getHeight() const;

		const RenderFontInterface * getFont() const;

	public:
		void loader( TiXmlElement * _xml ) override;

	protected:
		bool _compile();
		void _release();

	private:
		std::string m_fontName;

		RenderFontInterface * m_font;
	};
}