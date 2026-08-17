#pragma once

#include "GOAP/NodeInterface.h"

#include "Kernel/Affector.h"
#include "Kernel/Transformable.h"
#include "Kernel/Vector.h"

#include "math/vec3.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum ETransformationPathMode
    {
        ETPM_LINEAR,
        ETPM_BEZIER,
        ETPM_CATMULL_ROM
    };
    //////////////////////////////////////////////////////////////////////////
    class AffectorTransformationPath
        : public Affector
    {
    public:
        AffectorTransformationPath( GOAP::NodeInterface * _node, const TransformablePtr & _transformable, const Vector<mt::vec3f> & _points, float _duration, ETransformationPathMode _mode );
        ~AffectorTransformationPath() override;

    protected:
        bool _affect( const UpdateContext * _context, float * const _used ) override;
        void _complete( bool _isEnd ) override;

    protected:
        void evaluate_( float _t, mt::vec3f * const _position ) const;
        void evaluateLinear_( float _t, mt::vec3f * const _position ) const;
        void evaluateBezier_( float _t, mt::vec3f * const _position ) const;
        void evaluateCatmullRom_( float _t, mt::vec3f * const _position ) const;

    protected:
        GOAP::NodeInterfacePtr m_node;
        TransformablePtr m_transformable;
        Vector<mt::vec3f> m_points;
        float m_duration;
        float m_elapsed;
        ETransformationPathMode m_mode;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AffectorTransformationPath> AffectorTransformationPathPtr;
    //////////////////////////////////////////////////////////////////////////
}
