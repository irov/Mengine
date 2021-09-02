#pragma once

#include "Interface/ProfilerDescriptionInterface.h"

#include "optick.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class OptickProfilerDescription
        : public ProfilerDescriptionInterface
    {
        DECLARE_FACTORABLE( OptickProfilerDescription );

    public:
        OptickProfilerDescription();
        ~OptickProfilerDescription() override;

    public:
        const Char * getName() const;
        const Char * getFile() const;
        uint32_t getLine() const;

    public:
        bool initialize( const Char * _name, const Char * _file, uint32_t _line );

    public:
        Optick::EventDescription * getOptickDescription() const;

    protected:
        Optick::EventDescription * m_description;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<OptickProfilerDescription, ProfilerDescriptionInterface> OptickProfilerDescriptionPtr;
    //////////////////////////////////////////////////////////////////////////
}