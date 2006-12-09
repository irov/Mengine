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
		std::string	m_currentNamePlayList;
		FADE_STATES	m_fadeState;
		Playlist * m_currentPlayList;
	private:
		SoundSourceInterface* m_soundSource;
		FileDataInterface* m_fileData;	
		void	_beginFade();
	};
};