#	pragma once

#   include "Interface/RenderSystemInterface.h"

#	include "Kernel/ResourceReference.h"

#	include "Math/vec4.h"
#	include "Core/ColourValue.h"

#   include "Glyph.h"

#	include <vector>
#	include <map>

namespace Menge
{
	class ResourceGlyph;
	
	class ResourceFont
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceFont )

	public:
		ResourceFont();

	public:
		const ResourceGlyph * getResourceGlyph() const;

	public:
		bool hasGlyph( GlyphChar _id, const Glyph ** _glyph ) const;
		const Glyph * getGlyph( GlyphChar _id ) const;

    public:
		const RenderTextureInterfacePtr & getTextureFont() const;
		const RenderTextureInterfacePtr & getTextureOutline() const;

		const mt::vec4f&  getTextureUV() const;

		const FilePath & getImagePath() const;
		const FilePath & getOutlineImagePath() const;

		const ColourValue & getColor() const;

	public:
		bool _loader( const Metabuf::Metadata * _parser ) override;

	protected:
		bool _compile() override;
		void _release() override;

	private:
		void setImagePath_( const FilePath & _path );
		void setOutlineImagePath_( const FilePath & _path );
		void updateTextureUV_();

	private:
		ConstString m_resourceGlyphName;
		ResourceGlyph * m_resourceGlyph;
		
		FilePath m_fontImageFile;
		ConstString m_fontImageCodec;

		//String m_fontdefPath;

		FilePath m_outlineImageFile;
		ConstString m_outlineImageCodec;

		float m_textureRatio;

		mt::vec4f m_textureUV;

		RenderTextureInterfacePtr m_textureFont;
		RenderTextureInterfacePtr m_textureOutline;

		ColourValue m_color;
	};
}
