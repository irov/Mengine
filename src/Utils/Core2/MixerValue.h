#pragma once

#include "Core/ConstString.h"
#include "Core/ValueFollower.h"

#include "Config/Vector.h"

namespace Mengine
{
	class MixerValue
	{
	public:
		MixerValue();

    public:
        void setSpeed( float _speed );
        float getSpeed() const;

        void setDefault( float _default );
        float getDefault() const;

	public:
		void setValue( const ConstString & _type, float _value, float _from, bool _force );
		float getValue( const ConstString & _type ) const;

	public:
		float mixValue() const;

	public:
		bool update( float _current, float _timing );

	protected:
        float m_speed;

		struct Element
		{
			ConstString type;
			ValueFollowerLinear<float> follower;
		};

		typedef Vector<Element> TMixerElement;
		TMixerElement m_mixer;
	};
}