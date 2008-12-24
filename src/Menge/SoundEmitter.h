#	pragma once

#	include "Node.h"

#	include "Interface/SoundSystemInterface.h"

namespace Menge
{
	class	ResourceSound;

	class	SoundEmitter
		: public Node, public SoundNodeListenerInterface
	{
			OBJECT_DECLARE(SoundEmitter);
		public:
			SoundEmitter();
			~SoundEmitter();
		public:
			void loader( XmlElement * _xml ) override;

		public:
			void play();
			void pause();
			void stop();

			bool isPlaying();

			void setVolume( float _volume );
			float getVolume();
			void updateVolume();
 
			void setLooped( bool _loop );
			bool isLooping();

			float getLengthMs();

			void setSoundResource( const String& _name );

			void	listenPaused();
			void	listenStopped();

		protected:
			bool _activate() override;
			void _deactivate() override;

			bool _compile() override;
			void _release() override;
			void _setListener() override;

		private:
			ResourceSound * m_resource;
			String m_resourcename;
			
			unsigned int m_sourceID;

			bool m_isHeadMode;
			bool m_looped;
			bool m_playing;
	};
};