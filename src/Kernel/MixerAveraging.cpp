#include "MixerAveraging.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MixerAveraging::MixerAveraging()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MixerAveraging::~MixerAveraging()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void MixerAveraging::setValue( const ConstString & _type, float _value, float _weight, float _from, float _speed, bool _force )
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
            mixer.weight = _weight;

            return;
        }

        Element el;
        el.type = _type;
        el.follower.setValue( _from );
        el.follower.setFollow( _value );
        el.follower.setSpeed( _speed );
        el.weight = _weight;

        m_mixer.emplace_back( el );
    }
    //////////////////////////////////////////////////////////////////////////
    float MixerAveraging::getValue( const ConstString & _type ) const
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

        return 0.f;
    }
    //////////////////////////////////////////////////////////////////////////
    float MixerAveraging::mixValue() const
    {
        if( m_mixer.empty() == true )
        {
            return 0.f;
        }

        float mix = 0.f;
        float total = 0.f;

        for( const Element & mixer : m_mixer )
        {
            float value = mixer.follower.getValue();
            float weight = mixer.weight;

            mix += value;
            total += weight;
        }

        mix /= total;

        return mix;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MixerAveraging::update( const UpdateContext * _context )
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