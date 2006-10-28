#	pragma once

#	include "Renderable.h"

#	include "math/mat3.h"

#	include <vector>

#	include "..\MngReader\MNG.h"

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

		void stop();
		void play();
		void setLooped(bool _looped);
		bool getLooped() const;
		void setFirstFrame();
		void update(float _timing);

	public:
		void loader(TiXmlElement *xml) override;
		void render() override;

	public:
		bool _compile() override;
		void _release() override;

	public:
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
	};
}