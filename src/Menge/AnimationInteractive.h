#	pragma once

#	include "Animation.h"
#	include "SoundEmitter.h"

namespace Menge
{
	class AnimationInteractive
		: public Animation
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
		SoundEmitter * m_soundEmitter;
	};
}