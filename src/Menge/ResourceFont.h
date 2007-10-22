#	pragma once

#	include "ResourceImpl.h"

#	include "Math/vec4.h"
#	include <vector>

class RenderFontInterface;

namespace Menge
{
	class ResourceFont
		: public ResourceImpl
	{
	public:
		ResourceFont( const std::string & _name );

	public:
		RenderFontInterface * getFont() const;

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