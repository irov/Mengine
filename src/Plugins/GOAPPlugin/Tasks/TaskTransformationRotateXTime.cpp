#include "TaskTransformationRotateXTime.h"

#include "AffectorTransformationRotateTime.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Delegate.h"

#include "math/angle.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskTransformationRotateXTime::TaskTransformationRotateXTime( const TransformationPtr & _transformation, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, float _to, float _time, ETransformationRotateMode _mode, const DocumentPtr & _doc )
        : m_transformation( _transformation )
        , m_affectorable( _affectorable )
        , m_easing( _easing )
        , m_to( _to )
        , m_time( _time )
        , m_mode( _mode )
        , m_doc( _doc )
        , m_id( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskTransformationRotateXTime::~TaskTransformationRotateXTime()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskTransformationRotateXTime::_onRun( GOAP::NodeInterface * _node )
    {
        float orientationX = m_transformation->getLocalOrientationX();

        LambdaAffectorTransformationRotateTime transformCb = Helper::delegate( m_transformation, &Transformation::setLocalOrientationX );

        AffectorPtr affector = Helper::makeFactorableUnique<AffectorTransformationRotateTime>( m_doc, _node, transformCb, orientationX, m_to, m_time, m_mode );

        affector->setEasing( m_easing );

        const AffectorHubInterfacePtr & affectorHub = m_affectorable->getAffectorHub();

        UniqueId id = affectorHub->addAffector( affector );

        if( id == 0 )
        {
            return true;
        }

        m_id = id;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskTransformationRotateXTime::_onSkip()
    {
        if( m_id != 0 )
        {
            const AffectorHubInterfacePtr & affectorHub = m_affectorable->getAffectorHub();

            affectorHub->stopAffector( m_id );
            m_id = 0;
        }

        m_transformation->setLocalOrientationX( m_to );
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskTransformationRotateXTime::_onFinally()
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