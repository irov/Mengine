#	 pragma once

#	include "Config/Typedef.h"

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

	//! Amplifier - ����������� ������. 
	/*! 
	*
	*/

	class Amplifier
		:	public SoundNodeListenerInterface
	{
		public:
			//! �����������. ���������� ��������� 100%.
			/*!
			*/
			Amplifier();

			//! ����������.
			/*!
			*/
			~Amplifier();

			//! ������������ ���������. �������� �������� ��� ������.
			/*!
			\param _playlist ��� �������.
			*/
			void	playAllTracks( const String& _playlistResource );

			//! Shuffle ���������.
			/*!
			\param _playlist ��� ���������.
			*/
			void	shuffle( const String& _playlist );

			//! ��������� ������.
			/*!
			*/
			void	stop();

			const String& getPlaying() const;

			void volumeTo( float _time, float _value );
			void volumeToCb( float _time, float _value, PyObject* _cb );

			std::size_t getNumTracks() const;

			void update( float _timing );

			void playTrack( const String& _playlistResource, int _index, bool _looped );

			float getPosMs();
			void setPosMs( float _posMs );

		private:
			float m_volume;
			float m_volumeOverride;

			typedef	std::map< String, Playlist* >	TMapPlayList;

			TMapPlayList	m_mapPlayLists;

			String	m_currentPlaylistName;
			Playlist *	m_currentPlayList;
			
			unsigned int m_sourceID;
			SoundBufferInterface * m_buffer;

			void	listenPaused();
			void	listenStopped();
			void	_release();	
			void	_prepareSound( const String& _pakName, const String& _filename );
			bool	_loadPlayList( const String& _playlistResource );

			ValueInterpolatorLinear<float> m_volumeTo;
			PyObject* m_volToCb;
			bool m_playing;
			bool m_needRefocus;
	};
}
