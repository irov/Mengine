#	pragma once

#	include "SceneNode2D.h"

#	include <vector>

class RenderFontInterface;

namespace Menge
{
	class ResourceFont;

	class TextField
		: public SceneNode2D
	{
		OBJECT_DECLARE(TextField);
	public:
		TextField();
		~TextField();

	public:
		void setMessage( const std::string& _message );

	public:
		bool isVisible( const Viewport & _viewPort ) override;
		void loader( TiXmlElement * _xml ) override;

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
		mt::vec2f	m_length;
	};
}