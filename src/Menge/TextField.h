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
		void setHeight( float _height );
		void setColor( unsigned int _color );

		float getHeight() const;
		unsigned int getColor() const;
		const std::string & getText() const;

	public:
		bool isVisible( const Viewport & _viewPort ) override;
		void loader( TiXmlElement * _xml ) override;

	protected:
		void _render() override;
		void _debugRender() override;

		bool _activate() override;
		void _deactivate() override;
		
		bool _compile() override;
		void _release() override;

	private:
		ResourceFont * m_resourceFont;

		unsigned int m_color;
		float m_height;
		std::string m_text;
		std::string m_resourceFontName;
		mt::vec2f	m_length;
	};
}