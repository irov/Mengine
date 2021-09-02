#include "OptickProfilerDescription.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OptickProfilerDescription::OptickProfilerDescription()
        : m_description( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OptickProfilerDescription::~OptickProfilerDescription()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * OptickProfilerDescription::getName() const
    {
        return m_description->name;
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * OptickProfilerDescription::getFile() const
    {
        return m_description->file;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t OptickProfilerDescription::getLine() const
    {
        return m_description->line;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OptickProfilerDescription::initialize( const Char * _name, const Char * _file, uint32_t _line )
    {
        uint8_t flags = 0;

        if( _name != nullptr )
        {
            flags |= ::Optick::EventDescription::IS_CUSTOM_NAME;
        }

        m_description = Optick::EventDescription::Create( _name, _file, _line, Optick::Category::GetColor( Optick::Category::Type::None ), Optick::Category::GetMask( Optick::Category::Type::None ), flags );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    Optick::EventDescription * OptickProfilerDescription::getOptickDescription() const
    {
        return m_description;
    }
    //////////////////////////////////////////////////////////////////////////
}