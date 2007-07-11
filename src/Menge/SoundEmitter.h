#	pragma once

#	include "SceneNode2D.h"

#	include "Interface/SoundSystemInterface.h"

#	include <vector>

namespace Menge
{
	class	ResourceSound;

	class	SoundEmitter
		: public SceneNode2D
	{
			OBJECT_DECLARE(SoundEmitter);
		public:
			SoundEmitter();
			~SoundEmitter();
		public:
			void loader(TiXmlElement *xml) override;

		public:
			void play();
			void pause();
			void stop();

			bool isPlaying() const;

			void setVolume(float vol);
			float getVolume() const;

			void setLoop(bool loop);
			bool isLooping() const;

			int	getLengthMS();

		protected:
			bool _activate() override;
			void _deactivate() override;
		private:
			std::string m_resourceName;
			ResourceSound * m_resourceSound;
			SoundSourceInterface * m_interface;
			bool m_isHeadMode;

			void _changePivot() override; 
	};
};