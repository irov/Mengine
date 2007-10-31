#	pragma once

#	include "SceneNode2D.h"

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
		void setText( const std::string & _text );
		void setColor( unsigned int _color );

	public:
		bool isVisible( const Viewport & _viewPort ) override;
		void loader( TiXmlElement * _xml ) override;

	protected:
		void _render( const mt::mat3f &rwm, const Viewport & _viewPort ) override;
		void _debugRender() override;

		bool _activate() override;
		void _deactivate() override;
		
		bool _compile() override;
		void _release() override;

	private:
		ResourceFont * m_resourceFont;

		unsigned int m_color;
		std::string m_text;
		std::string m_resourceFontName;
		mt::vec2f	m_length;
	};
}