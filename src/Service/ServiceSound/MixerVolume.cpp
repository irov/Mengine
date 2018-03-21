#include "MixerVolume.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	MixerVolume::MixerVolume()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void MixerVolume::setVolume( const ConstString & _type, float _value, float _default )
	{
		for( TMixerVolume::iterator
			it = m_mixer.begin(),
			it_end = m_mixer.end();
		it != it_end;
		++it )
		{
			Mixer & mixer = *it;

			if( mixer.type != _type )
			{
				continue;
			}

			mixer.follower.follow( _value );

			return;
		}

		Mixer mx;
		mx.type = _type;
		mx.follower.setValue( _default );
		mx.follower.follow( _value );
		mx.follower.setSpeed( 0.0005f );

		m_mixer.push_back( mx );
	}
	//////////////////////////////////////////////////////////////////////////
	float MixerVolume::getVolume( const ConstString & _type ) const
	{
		for( TMixerVolume::const_iterator
			it = m_mixer.begin(),
			it_end = m_mixer.end();
		it != it_end;
		++it )
		{
			const Mixer & mixer = *it;

			if( mixer.type != _type )
			{
				continue;
			}

			float value = mixer.follower.getValue();

			return value;
		}

		return 1.f;
	}
	//////////////////////////////////////////////////////////////////////////
	float MixerVolume::mixVolume() const
	{
		float volume = 1.f;

		for( TMixerVolume::const_iterator
			it = m_mixer.begin(),
			it_end = m_mixer.end();
		it != it_end;
		++it )
		{
			const Mixer & mixer = *it;

			float value = mixer.follower.getValue();

			volume *= value;
		}

		return volume;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MixerVolume::update( float _timing )
	{
		bool process = false;

		for( TMixerVolume::iterator
			it = m_mixer.begin(),
			it_end = m_mixer.end();
		it != it_end;
		++it )
		{
			Mixer & m = *it;

			if( m.follower.update( _timing ) == false )
			{
				process = true;
			}
		}

		return process;
	}
}