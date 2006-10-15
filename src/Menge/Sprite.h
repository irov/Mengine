#	pragma once

#	include "Renderable.h"

#	include "math/mat3.h"

class RenderImageInterface;

namespace Menge
{
	struct SpriteDesc
	{
		std::string imageFile;
		bool haveAlpha;
	};

	class Sprite
		: public Renderable
	{
		OBJECT_DECLARE(Sprite);
	public:
		Sprite();

	public:
		void loader(TiXmlElement *xml)override;
		void render()override;

	public:
		bool _compile()override;
		void _release()override;

	public:
		void _debugRender()override;

	private:
		SpriteDesc *m_spriteDesc;
		RenderImageInterface* m_image;
	};
}