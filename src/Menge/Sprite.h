#	pragma once

#	include "Renderable.h"

#	include "math/mat3.h"

#	include <vector>

//#	include "CodecInterface.h"

//#	include "Decoder.h"

class RenderImageInterface;

namespace Menge
{
	class ResourceImage;

	class Sprite
		: public Renderable
	{
		OBJECT_DECLARE(Sprite);
	public:
		Sprite();
		~Sprite();

	public:
		virtual void setImageIndex( size_t _index );
		virtual size_t getImageIndex() const;
		virtual void setOffset(const mt::vec2f& _offset);

		virtual void setImageResource( const std::string & _name );
		virtual const std::string & getImageResource() const;

	public:
		virtual bool isVisible(const Viewport & _viewPort);

		void loader(TiXmlElement *xml) override;

	protected:
		bool _activate() override;
		void _deactivate() override;

		void _render( const mt::mat3f &rwm, const Viewport & _viewPort ) override;
		void _debugRender() override;
	
	protected:
		ResourceImage * m_image;
		std::string m_resourceName;

		size_t m_currentImageIndex;

		mt::vec2f m_offset;		
	
		unsigned int m_color;
	};
}