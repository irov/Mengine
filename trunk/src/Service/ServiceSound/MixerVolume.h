#	pragma once

#	include "Core/ConstString.h"

#	include <stdex/stl_vector.h>

namespace Menge
{
	class MixerVolume
	{
	public:
		void setVolume( const ConstString & _type, float _value );
		float getVolume( const ConstString & _type ) const;

	public:
		float mixVolume() const;

	protected:
		struct Mixer
		{
			ConstString type;
			float value;
		};

		typedef stdex::vector<Mixer> TMixerVolume;
		TMixerVolume m_mixer;
	};
}