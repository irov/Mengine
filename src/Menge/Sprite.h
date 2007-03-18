#	pragma once

#	include "Renderable.h"

#	include "math/mat3.h"

#	include <vector>

#	include "SpriteDecoder.h"

class RenderImageInterface;

namespace Menge
{
	enum	eAnimState
	{
		FORWARD,	
		REWIND,	
	};

	class Sprite
		: public Renderable
	{
		OBJECT_DECLARE(Sprite);
	public:
		Sprite();

		virtual void play();
		virtual void stop();
	
		
		virtual void setLooped(bool _looped);
		virtual bool getLooped() const;
		
		void setFirstFrame();

		void setOffset(const mt::vec2f& _offset);

		bool isVisible(const Viewport & _viewPort);

	public:
		void loader(TiXmlElement *xml) override;

	protected:
		void _update(float _timing) override;
		bool _compile() override;
		void _release() override;
		void _render( const mt::mat3f &rwm, const Viewport & _viewPort ) override;
		void _debugRender() override;
	
	private:
		bool m_playing;
		bool m_looping;
		eAnimState	m_state;
		std::string m_fileMNG;

	private:
		SpriteDecoder::SpriteData*	m_sprData;
		struct Frame 
		{
			int		index;
			int		delay;
		};

		struct Image
		{
			mt::vec2f offset;
			RenderImageInterface* renderImage;
		};

		std::vector<Frame>	m_frames;
		std::vector<Frame>::iterator m_currentFrame;

		std::vector<Image>	m_images;

		float	m_ctdelay;
		float   m_width;
		float   m_height;

		mt::vec2f m_offset;
	};
}