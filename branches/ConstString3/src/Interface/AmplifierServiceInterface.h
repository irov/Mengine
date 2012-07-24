#	pragma once

#	include "Interface/ServiceInterface.h"

#	include "Utils/Core/ConstString.h"

namespace Menge
{
	class AmplifierServiceInterface
		: public ServiceInterface
	{
	public:
		virtual void playTrack( const ConstString& _playlistResource, int _index, bool _looped ) = 0;
		virtual void playAllTracks( const ConstString& _playlistResource ) = 0;
		virtual void resetPlayList() = 0;
		
		virtual const ConstString& getPlayTrack() const = 0;
		virtual size_t getNumTracks() const = 0;

		virtual void shuffle( const ConstString& _playlist ) = 0;

		virtual void play() = 0;
		virtual void stop() = 0;
		virtual void pause() = 0;
		virtual void resume() = 0;
		
		virtual void setVolume( float _value ) = 0;		
		
		virtual void onTurnSound( bool _turn ) = 0;

		virtual void setPosMs( float _posMs ) = 0;
		virtual float getPosMs() const = 0;
	};
}