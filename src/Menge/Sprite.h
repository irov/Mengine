#	pragma once

#	include "Renderable.h"

#	include "math/mat3.h"

#	include <vector>

#	include "CodecInterface.h"

#	include "Decoder.h"

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

	public:
		virtual void setFrame( size_t _frame );
		virtual size_t getFrame() const;
		virtual void setOffset(const mt::vec2f& _offset);

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

		size_t m_currentFrame;

		mt::vec2f m_offset;		
	
		unsigned int m_color;
	};
}