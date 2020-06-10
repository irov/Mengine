#include "TaskTransformationRotateZ.h"

#include "AffectorTransformationRotate.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Delegate.h"

#include "math/angle.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskTransformationRotateZ::TaskTransformationRotateZ( const TransformationPtr & _transformation, const AffectorablePtr & _affectorable, float _to, float _speed )
        : m_transformation( _transformation )
        , m_affectorable( _affectorable )
        , m_to( _to )
        , m_speed( _speed )
        , m_id( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskTransformationRotateZ::~TaskTransformationRotateZ()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskTransformationRotateZ::_onRun( GOAP::NodeInterface * _node )
    {
        float orientationZ = m_transformation->getLocalOrientationZ();

        float correct_rotate_from;
        float correct_rotate_to;
        mt::angle_correct_interpolate_from_to( orientationZ, m_to, correct_rotate_from, correct_rotate_to );

        LambdaAffectorTransformationRotate transformCb = Helper::delegate( m_transformation, &Transformation::setLocalOrientationZ );

        AffectorPtr affector = Helper::makeFactorableUnique<AffectorTransformationRotate>( MENGINE_DOCUMENT_FUNCTION, _node, transformCb, correct_rotate_from, correct_rotate_to, m_speed );

        const AffectorHubInterfacePtr & affectorHub = m_affectorable->getAffectorHub();

        AFFECTOR_ID id = affectorHub->addAffector( affector );

        if( id == 0 )
        {
            return true;
        }

        m_id = id;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskTransformationRotateZ::_onSkip()
    {
        if( m_id != 0 )
        {
            const AffectorHubInterfacePtr & affectorHub = m_affectorable->getAffectorHub();

            affectorHub->stopAffector( m_id );
            m_id = 0;
        }

        m_transformation->setLocalOrientationZ( m_to );
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskTransformationRotateZ::_onFinally()
    {
        if( m_id != 0 )
        {
            const AffectorHubInterfacePtr & affectorHub = m_affectorable->getAffectorHub();

            affectorHub->stopAffector( m_id );
            m_id = 0;
        }

        m_affectorable = nullptr;
        m_transformation = nullptr;
    }
}