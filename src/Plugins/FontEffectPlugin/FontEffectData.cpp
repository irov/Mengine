#include "FontEffectData.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FontEffectData::FontEffectData()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FontEffectData::~FontEffectData()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectData::acquire()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectData::release()
    {
        m_entries.clear();
        m_order.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectData::addEffect( const ConstString & _name, const FontEffectDesc & _desc, uint32_t _sample )
    {
        Entry entry;
        entry.desc = _desc;
        entry.sample = _sample;

        if( m_entries.find( _name ) == m_entries.end() )
        {
            m_order.emplace_back( _name );
        }

        m_entries[_name] = entry;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectData::findEffect( const ConstString & _name, FontEffectDesc * const _desc, uint32_t * const _sample ) const
    {
        MapEntries::const_iterator it_found = m_entries.find( _name );

        if( it_found == m_entries.end() )
        {
            return false;
        }

        const Entry & entry = it_found->second;

        if( _desc != nullptr )
        {
            *_desc = entry.desc;
        }

        if( _sample != nullptr )
        {
            *_sample = entry.sample;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectData::foreachEffect( const LambdaFontEffectEntry & _lambda ) const
    {
        for( const ConstString & name : m_order )
        {
            MapEntries::const_iterator it_found = m_entries.find( name );

            const Entry & entry = it_found->second;

            _lambda( name, entry.desc, entry.sample );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
