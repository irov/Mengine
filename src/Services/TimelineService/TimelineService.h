#pragma once

#include "Interface/TimelineServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    class TimelineService
        : public ServiceBase<TimelineServiceInterface>
    {
    public:
        TimelineService();
        ~TimelineService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void beginOffset( float _offset, const DocumentPtr & _doc ) override;
        float getOffset() const override;
        void endOffset() override;

    public:
        uint32_t getCurrentRevision() const override;
        float getCurrentTime() const override;
        float getCurrentDelta() const override;

    public:
        float getTotalTime() const override;

    public:
        void begin( const UpdateContext * _context ) override;
        void end() override;

    protected:
        uint32_t m_revision;
        float m_current;
        float m_time;

        float m_total;

        typedef Vector<float> VectorTime;
        VectorTime m_offsets;
    };
};
