#	pragma once

#	include "Renderable.h"

#	include "math/mat3.h"

#	include <vector>

/*	добавлен всякий ахтунг	*/
#	include "..\MngReader\MNG.h"
/**/

class RenderImageInterface;

namespace Menge
{
	struct SpriteDesc
	{
		std::string imageFile;
		bool haveAlpha;
	};

	enum	ANIMSTATE
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
		
		void setLooped(bool flag);
		bool getLooped() const;
		void update(float timing);
		

	public:
		void loader(TiXmlElement *xml) override;
		void render() override;

	public:
		bool _compile() override;
		void _release() override;

	public:
		void _debugRender()override;

	private:
	private:
		bool m_playing;
		bool m_looping;

		bool m_haveAlpha;

		ANIMSTATE	m_state;

		std::string m_fileMNG;

		std::vector<mnglib::Frame>::iterator m_currentFrame;
		std::vector<RenderImageInterface*>	m_renderImages;
		mnglib::mngDesc	m_desc;
		float m_currentDelay;
		//RenderImageInterface* m_image;
	};
}