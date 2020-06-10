#include "TaskTransformationRotateZTime.h"

#include "AffectorTransformationRotateTime.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Delegate.h"

#include "math/angle.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskTransformationRotateZTime::TaskTransformationRotateZTime( const TransformationPtr & _transformation, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, float _to, float _time, ETransformationRotateMode _mode )
        : m_transformation( _transformation )
        , m_affectorable( _affectorable )
        , m_easing( _easing )
        , m_to( _to )
        , m_time( _time )
        , m_mode( _mode )
        , m_id( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskTransformationRotateZTime::~TaskTransformationRotateZTime()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskTransformationRotateZTime::_onRun( GOAP::NodeInterface * _node )
    {
        float orientationZ = m_transformation->getLocalOrientationZ();

        LambdaAffectorTransformationRotateTime transformCb = Helper::delegate( m_transformation, &Transformation::setLocalOrientationZ );

        AffectorPtr affector = Helper::makeFactorableUnique<AffectorTransformationRotateTime>( MENGINE_DOCUMENT_FUNCTION, _node, transformCb, orientationZ, m_to, m_time, m_mode );

        affector->setEasing( m_easing );

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
    void TaskTransformationRotateZTime::_onSkip()
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
    void TaskTransformationRotateZTime::_onFinally()
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