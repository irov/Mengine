#	include "MixerVolume.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	void MixerVolume::setVolume( const ConstString & _type, float _value )
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

			mixer.value = _value;

			return;
		}

		Mixer mx;
		mx.type = _type;
		mx.value = _value;

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

			return mixer.value;
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

			volume *= mixer.value;
		}

		return volume;
	}
}