#	pragma once

#	include "Renderable.h"

#	include "math/mat3.h"

#	include <vector>

#	include "CodecInterface.h"

#	include "Decoder.h"

class RenderImageInterface;

namespace Menge
{
	enum	eAnimState
	{
		FORWARD,	
		REWIND,	
		//PINGPONG,	//надо ли нам такой стейт? т.е. анимация играется сначало вперед, потом назад.
	};

	class ResourceImage;

	class Sprite
		: public Renderable
	{
		OBJECT_DECLARE(Sprite);
	public:
		Sprite();

	public:
		virtual void play();
		virtual void stop();
		virtual void setLooped(bool _looped);
		virtual void setFirstFrame();
		virtual void nextFrame();
		virtual void prevFrame();
		virtual void setOffset(const mt::vec2f& _offset);
		virtual bool getLooped() const;

	public:
		virtual bool isVisible(const Viewport & _viewPort);

		void loader(TiXmlElement *xml) override;

	protected:
		void _update(float _timing) override;
		
		bool _activate() override;
		void _deactivate() override;

		void _render( const mt::mat3f &rwm, const Viewport & _viewPort ) override;
		void _debugRender() override;
	
	private:
		ResourceImage * m_image;
		std::string m_resourceName;

		mt::vec2f m_offset;
		size_t m_currentFrame;

		float	m_total_delay;

		bool m_playing;
		bool m_looping;

		eAnimState	m_state;
	};
}