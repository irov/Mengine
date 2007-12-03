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
			void loader( TiXmlElement * _xml ) override;

		public:
			void play();
			void pause();
			void stop();

			bool isPlaying();

			void setVolume( float _volume );
			float getVolume();
 
			void setLooped( bool _loop );
			bool isLooping();

			int	getLengthMs();

			void setSoundListener( PyObject * _listener );

			void setSoundResource( const std::string & _name );

			void	listenPaused( bool _pause );
			void	listenStopped();

		protected:
			bool _activate() override;
			void _deactivate() override;

			bool _compile() override;
			void _release() override;

		private:
			PyObject * m_listener;

			ResourceSound * m_resource;
			std::string m_resourcename;
			
			SoundSourceInterface * m_interface;

			bool m_isHeadMode;
	};
};