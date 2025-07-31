#pragma once

#include "GOAP/NodeInterface.h"

#include "TransformationRotateMode.h"

#include "Kernel/Affector.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<void( float )> LambdaAffectorTransformationRotateTime;
    //////////////////////////////////////////////////////////////////////////
    class AffectorTransformationRotateTime
        : public Affector
    {
    public:
        AffectorTransformationRotateTime( GOAP::NodeInterface * _node, const LambdaAffectorTransformationRotateTime & _transformCb, float _from, float _to, float _time, ETransformationRotateMode _mode );
        ~AffectorTransformationRotateTime() override;

    public:
        bool _affect( const UpdateContext * _context, float * const _used ) override;

    protected:
        void _prepare() override;
        void _complete( bool _isEnd ) override;

    protected:
        GOAP::NodeInterfacePtr m_node;

        LambdaAffectorTransformationRotateTime m_transformCb;

        float m_from;
        float m_to;
        float m_progress;
        float m_time;
        ETransformationRotateMode m_mode;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AffectorTransformationRotateTime> AffectorTransformationRotateTimePtr;
    //////////////////////////////////////////////////////////////////////////
}