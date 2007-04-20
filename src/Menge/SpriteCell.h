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

	class SpriteCell
		: public Renderable
	{
		OBJECT_DECLARE(SpriteCell);
	public:
		SpriteCell();
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
		bool _compile() override;
		void _release() override;
		void _render( const mt::mat3f &rwm, const Viewport & _viewPort ) override;
		void _debugRender() override;
	
	private:

		int m_numX;
		int m_numY;

		int m_cellWidth;
		int m_cellHeight;

		int m_imageWidth;
		int m_imageHeight;

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

		std::vector<Frame>	frames;
		std::vector<Frame>::iterator m_currentFrame;

		std::vector<Image>	images;

		float	m_total_delay;
		mt::vec2f m_size;
		mt::vec2f m_offset;
		bool m_playing;
		bool m_looping;
		eAnimState	m_state;
		std::string m_fileMNG;
	};
}