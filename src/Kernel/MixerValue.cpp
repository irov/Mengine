#include "MixerValue.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MixerValue::MixerValue()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MixerValue::~MixerValue()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void MixerValue::setValue( const ConstString & _type, float _value, float _from, float _speed, bool _force )
    {
        for( Element & mixer : m_mixer )
        {
            if( mixer.type != _type )
            {
                continue;
            }

            if( _force == true )
            {
                mixer.follower.setValue( _from );
            }

            mixer.follower.setFollow( _value );
            mixer.follower.setSpeed( _speed );

            return;
        }

        if( _value == 1.f && _from == 1.f )
        {
            return;
        }

        Element el;
        el.type = _type;
        el.follower.setValue( _from );
        el.follower.setFollow( _value );
        el.follower.setSpeed( _speed );

        m_mixer.emplace_back( el );
    }
    //////////////////////////////////////////////////////////////////////////
    float MixerValue::getValue( const ConstString & _type ) const
    {
        for( const Element & mixer : m_mixer )
        {
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
    float MixerValue::mixValue() const
    {
        float mix = 1.f;

        for( const Element & mixer : m_mixer )
        {
            float value = mixer.follower.getValue();

            mix *= value;
        }

        return mix;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MixerValue::update( const UpdateContext * _context )
    {
        bool process = false;

        for( Element & m : m_mixer )
        {
            float used = 0.f;

            if( m.follower.update( _context, &used ) == false )
            {
                process = true;
            }
        }

        return process;
    }
    //////////////////////////////////////////////////////////////////////////
}