#	pragma once

#	include "Core/ConstString.h"
#	include "Core/ValueFollower.h"

#	include <stdex/stl_vector.h>

namespace Menge
{
	class MixerVolume
	{
	public:
		MixerVolume();

	public:
		void setVolume( const ConstString & _type, float _value, float _default );
		float getVolume( const ConstString & _type ) const;

	public:
		float mixVolume() const;

	public:
		bool update( float _timing );

	protected:
		struct Mixer
		{
			ConstString type;
			ValueFollowerLinear<float> follower;
		};

		typedef stdex::vector<Mixer> TMixerVolume;
		TMixerVolume m_mixer;
	};
}