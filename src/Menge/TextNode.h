#	pragma once

#	include "SceneNode2D.h"

#	include <vector>

class RenderFontInterface;

namespace Menge
{
	class ResourceFont;

	class TextNode
		: public SceneNode2D
	{
		OBJECT_DECLARE(TextNode);
	public:
		TextNode();
		~TextNode();

	public:
		virtual bool isVisible( const Viewport & _viewPort );

		void loader( TiXmlElement * _xml ) override;

	public:
		void	setMessage( const std::string& _message );

	protected:
		bool _activate() override;
		void _deactivate() override;

		void _render( const mt::mat3f &rwm, const Viewport & _viewPort ) override;
		void _debugRender() override;
	private:
		ResourceFont * m_resourceFont;
		RenderFontInterface * m_font;
		std::string m_message;
		std::string m_resourceFontName;
	};
}