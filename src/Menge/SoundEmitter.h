#	pragma once

#	include "SceneNode2D.h"

#	include "Interface/SoundSystemInterface.h"

#	include <vector>

namespace Menge
{
	class	ResourceSound;

	class	SoundEmitter
		: public SceneNode2D, public SoundNodeListenerInterface
	{
			OBJECT_DECLARE(SoundEmitter);
		public:
			SoundEmitter();
			~SoundEmitter();
		public:
			void loader(TiXmlElement * _xml) override;

		public:
			void play();
			void pause();
			void stop();

			bool isPlaying() const;

			void setVolume(float vol);
			float getVolume() const;

			void setLooped(bool loop);
			bool isLooping() const;

			int	getLengthMS();

			void setSoundListener( PyObject * _listener );

			void setSoundResource( const std::string & _name );

		protected:
			bool _activate() override;
			void _deactivate() override;

			bool _compile() override;
			void _release() override;

		private:
			PyObject * m_listener;

			std::string m_resourceName;
			ResourceSound * m_resourceSound;
			SoundSourceInterface * m_interface;
			bool m_isLooped;
			bool m_isHeadMode;

			void _changePivot() override; 

			void	listenPaused(bool _pause);
			void	listenStopped();
	};
};