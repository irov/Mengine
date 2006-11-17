#	pragma once

#	include "Sprite.h"
#	include "Sound.h"

namespace Menge
{
	class SpriteInteractive
		: public Sprite
	{
		OBJECT_DECLARE(SpriteInteractive)

	public:
		void play() override;
		void pause() override;
		void stop() override;		

		void setLooped(bool _looped) override;

	protected:
		void loader(TiXmlElement *_xml) override;

	private:
		Sound m_playSound;
	};
}