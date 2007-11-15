#	pragma once

#	include "SceneNode2D.h"

#	include "math/mat3.h"
#	include "math/vec4.h"

#	include <vector>

class RenderImageInterface;

namespace Menge
{
	class ResourceImage;

	class Sprite
		: public SceneNode2D
	{
		OBJECT_DECLARE(Sprite);
	public:
		Sprite();
		~Sprite();

	public:
		virtual void setImageIndex( size_t _index );
		virtual size_t getImageIndex() const;

		virtual void setImageResource( const std::string & _name );
		virtual const std::string & getImageResource() const;

		virtual void setPercentVisibility( const mt::vec4f & _percent );

		virtual void setScale( float _scale );
		virtual float getScale() const;

	public:
		virtual bool isVisible( const Viewport & _viewPort );

		void loader( TiXmlElement * _xml ) override;

	protected:
		void _render() override;
		void _debugRender() override;

		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;
	
	private:
		void updateAlign_();

	protected:
		ResourceImage * m_image;
		std::string m_resourceImage;

		size_t m_currentImageIndex;

		bool m_centerAlign;
		mt::vec2f m_alignOffset;

		float m_scale;

		mt::vec4f	m_percent;

		unsigned int m_color;
	};
}