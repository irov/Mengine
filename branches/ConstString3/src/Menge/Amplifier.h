#	 pragma once

#	include "Core/ConstString.h"

#	include "Config/Typedef.h"
#	include "Core/Holder.h"

#	include "Interface/SoundSystemInterface.h"

#	include "ValueInterpolator.h"

#	include <map>

extern "C" 
{ 
	struct _object; 
	typedef _object PyObject;
}

namespace	Menge
{
	class ResourcePlaylist;
	class Playlist;

	//! Amplifier - проигрывает музыку. 
	/*! 
	*
	*/

	class Amplifier
		: public Holder<Amplifier> 
		, public SoundNodeListenerInterface	
	{
	public:
		Amplifier();
		~Amplifier();

	public:
		void playAllTracks( const ConstString& _playlistResource );

		void shuffle( const ConstString& _playlist );
		void stop();

		const ConstString& getPlaying() const;

		void volumeTo( float _time, float _value );
		void volumeToCb( float _time, float _value, PyObject* _cb );

		std::size_t getNumTracks() const;

		void update( float _timing );

		void playTrack( const ConstString& _playlistResource, int _index, bool _looped );

		float getPosMs();
		void setPosMs( float _posMs );

	private:
		float m_volume;
		float m_volumeOverride;

		typedef	std::map<ConstString, Playlist *>	TMapPlayList;

		TMapPlayList m_mapPlayLists;

		ConstString	m_currentPlaylistName;
		Playlist * m_currentPlayList;

		unsigned int m_sourceID;
		SoundBufferInterface * m_buffer;

		ValueInterpolatorLinear<float> m_volumeTo;
		PyObject* m_volToCb;
		bool m_playing;
		bool m_needRefocus;

	private:
		void	listenPaused();
		void	listenStopped();
		void	release_();	
		void	prepareSound_( const ConstString& _pakName, const String& _file, const ConstString& _codec );
		bool	loadPlayList_( const ConstString& _playlistResource );

	};
}
