#	pragma once

#	include "SceneNode2D.h"

#	include "Interface/SoundSystemInterface.h"

#	include <vector>

namespace Menge
{
	class	ResourceSound;

	class	SoundGroup
		: public SceneNode2D, public SoundNodeListenerInterface
	{
			OBJECT_DECLARE(SoundGroup);
		public:
			SoundGroup();
			~SoundGroup();
		public:
			void loader( TiXmlElement * _xml ) override;

		public:
			void play( const std::string & _name );
			void pause( const std::string & _name );
			void stop( const std::string & _name );

			bool isPlaying( const std::string & _name ) const;

			void setVolume( float _vol );
			float getVolume() const;

			void setLooped( bool _loop );
			bool isLooping() const;

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
			bool m_isHeadMode;

			void _changePivot() override; 

			void	listenPaused(bool _pause);
			void	listenStopped();
	};
};