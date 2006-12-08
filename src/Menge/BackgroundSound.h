#	include <vector>

#	include "SoundEngine.h"

#	include "SoundNode.h"

#	include "PlayList.h"

#	include "SoundSystemInterface.h"

namespace	Menge
{
	enum	FADE_STATES
	{
		FADE_DOWN,
		FADE_UP,
		NO_FADE,
	};

	class	BackgroundSound	: public SoundNodeListenerInterface
	{
	public:
		BackgroundSound();
		~BackgroundSound();
		void	play(Playlist& _playList);
		void	update();
		bool	listenRecycled(SoundSourceInterface*	_sn);
		void	listenStoped(SoundSourceInterface*	_sn);
		void	listenEnded(SoundSourceInterface*	_sn);
	private:

		
		float m_fadeVelocity;
		SoundSourceInterface* m_soundSource;
		FileDataInterface* m_fileData;	
		std::string	m_currentNamePlayList;
		Playlist* m_currentPlayList;
		FADE_STATES	m_fadeState;
		std::string	m_currentSoundTrackName;
		void	_beginFade();
	};
};