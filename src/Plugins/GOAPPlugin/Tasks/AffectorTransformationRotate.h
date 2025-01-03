#pragma once

#include "GOAP/NodeInterface.h"

#include "Kernel/Affector.h"
#include "Kernel/Factorable.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<void( float )> LambdaAffectorTransformationRotate;
    //////////////////////////////////////////////////////////////////////////
    class AffectorTransformationRotate
        : public Affector
    {
    public:
        AffectorTransformationRotate( GOAP::NodeInterface * _node, const LambdaAffectorTransformationRotate & _transformCb, float _from, float _to, float _speed );
        ~AffectorTransformationRotate() override;

    public:
        bool _affect( const UpdateContext * _context, float * const _used ) override;

    protected:
        void _complete( bool _isEnd ) override;

    protected:
        GOAP::NodeInterfacePtr m_node;

        LambdaAffectorTransformationRotate m_transformCb;

        float m_from;
        float m_to;
        float m_progress;
        float m_speed;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AffectorTransformationRotate> AffectorTransformationRotatePtr;
    //////////////////////////////////////////////////////////////////////////
}