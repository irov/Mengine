#	pragma once

#	include "Node.h"

#	include "Interface/SoundSystemInterface.h"

namespace Menge
{
	class	ResourceSound;

	class	SoundEmitter
		: public Node
		, public SoundNodeListenerInterface
	{
	public:
		SoundEmitter();
		~SoundEmitter();

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

		void setSoundResource( const ConstString& _name );

		void listenPaused();
		void listenStopped();

	public:
		void loader( XmlElement * _xml ) override;
		void parser( BinParser * _parser ) override;

	protected:
		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;
		void _setListener( PyObject * _listener ) override;

	private:
		ResourceSound * m_resource;
		String m_resourcename;

		unsigned int m_sourceID;

		bool m_isHeadMode;
		bool m_looped;
		bool m_playing;
	};
};
