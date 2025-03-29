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
    int32_t OptickProfilerDescription::getLine() const
    {
        return m_description->line;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OptickProfilerDescription::initialize( const Char * _name, const Char * _file, int32_t _line )
    {
        uint8_t flags = 0;

        if( _name != nullptr )
        {
            flags |= ::Optick::EventDescription::IS_CUSTOM_NAME;
        }

        uint32_t color = Optick::Category::GetColor( Optick::Category::Type::None );
        uint32_t filter = Optick::Category::GetMask( Optick::Category::Type::None );

        m_description = Optick::EventDescription::Create( _name, _file, _line, color, filter, flags );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    Optick::EventDescription * OptickProfilerDescription::getOptickDescription() const
    {
        return m_description;
    }
    //////////////////////////////////////////////////////////////////////////
}