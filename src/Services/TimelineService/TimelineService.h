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
        void beginOffset( float _offset, const DocumentInterfacePtr & _doc ) override;
        float getOffset() const override;
        void endOffset() override;

    public:
        const UpdateContext * getCurrentContext() const override;

    public:
        float getTotalTime() const override;

    public:
        void begin( const UpdateContext * _context ) override;
        void end() override;

    public:        
        float calcTimeFactor() const override;

    public:
        void setTimeFactorBase( float _timeFactor ) override;
        float getTimeFactorBase() const override;
        void setTimeFactorCoefficient( float _timeFactorCoefficient ) override;
        float getTimeFactorCoefficient() const override;
        void setTimeFactorCount( int32_t _timeFactorCount ) override;
        int32_t getTimeFactorCount() const override;

    protected:
        UpdateContext m_context;

        float m_total;

        float m_timeFactorBase;
        float m_timeFactorCoefficient;
        int32_t m_timeFactorCount;

        typedef Vector<float> VectorTime;
        VectorTime m_offsets;
    };
};
