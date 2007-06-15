#	pragma once

#	include "Allocator.h"

#	include "SoundSystemInterface.h"

#	include <vector>

namespace Menge
{
	class	ResourceSound;

	class	SoundEmitter
		: public Allocator
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
			int m_isHeadMode;

			void _changePivot() override; 
	};
};