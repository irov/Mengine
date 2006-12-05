#	pragma once

#	include "Renderable.h"

#	include "math/mat3.h"

#	include <vector>

#	include "../MngReader/MNG.h"

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

		void setOffset( const mt::vec2f &_offset );

	public:
		void loader(TiXmlElement *xml) override;

	protected:
		void _update(float _timing) override;
		bool _compile() override;
		void _release() override;
		void _render() override;
		void _debugRender() override;
	
	private:
		bool m_playing;
		bool m_looping;
		eAnimState	m_state;
		std::string m_fileMNG;

	private:
		struct ImageProperties
		{
			mt::vec2f offset;
			RenderImageInterface* renderImage;
		};
		std::vector<mnglib::Frame>::iterator m_currentFrame;
		std::vector<ImageProperties> m_images;
		mnglib::mngDesc	m_desc;
		float m_ctdelay;

		mt::vec2f m_offset;
	};
}