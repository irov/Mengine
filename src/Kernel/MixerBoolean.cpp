#include "MixerBoolean.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MixerBoolean::MixerBoolean()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MixerBoolean::~MixerBoolean()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void MixerBoolean::setValue( const ConstString & _type, bool _value )
    {
        for( Element & mixer : m_mixer )
        {
            if( mixer.type != _type )
            {
                continue;
            }

            mixer.value = _value;

            return;
        }

        if( _value == false )
        {
            return;
        }

        Element el;
        el.type = _type;
        el.value = _value;

        m_mixer.emplace_back( el );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MixerBoolean::getValue( const ConstString & _type ) const
    {
        for( const Element & mixer : m_mixer )
        {
            if( mixer.type != _type )
            {
                continue;
            }

            bool value = mixer.value;

            return value;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MixerBoolean::mixValue() const
    {
        bool mix = false;

        for( const Element & mixer : m_mixer )
        {
            bool value = mixer.value;

            mix |= value;
        }

        return mix;
    }
    //////////////////////////////////////////////////////////////////////////
}