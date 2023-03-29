#include "TaskTransformationRotateXTime.h"

#include "Interface/TransformationInterface.h"

#include "AffectorTransformationRotateTime.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Delegate.h"

#include "math/angle.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskTransformationRotateXTime::TaskTransformationRotateXTime( GOAP::Allocator * _allocator, const TransformablePtr & _transformable, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, float _to, float _time, ETransformationRotateMode _mode, const DocumentPtr & _doc )
        : GOAP::TaskInterface( _allocator )
        , m_transformable( _transformable )
        , m_affectorable( _affectorable )
        , m_easing( _easing )
        , m_to( _to )
        , m_time( _time )
        , m_mode( _mode )
#if defined(MENGINE_DOCUMENT_ENABLE)
        , m_doc( _doc )
#endif
        , m_id( INVALID_UNIQUE_ID )
    {
        MENGINE_UNUSED( _doc );
    }
    //////////////////////////////////////////////////////////////////////////
    TaskTransformationRotateXTime::~TaskTransformationRotateXTime()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskTransformationRotateXTime::_onRun( GOAP::NodeInterface * _node )
    {
        TransformationInterface * transformation = m_transformable->getTransformation();

        float orientationX = transformation->getLocalOrientationX();

        LambdaAffectorTransformationRotateTime transformCb = Helper::delegate( transformation, &TransformationInterface::setLocalOrientationX );

        AffectorPtr affector = Helper::makeFactorableUnique<AffectorTransformationRotateTime>( MENGINE_DOCUMENT_VALUE( m_doc, nullptr ), _node, transformCb, orientationX, m_to, m_time, m_mode );

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
        if( m_id != INVALID_UNIQUE_ID )
        {
            const AffectorHubInterfacePtr & affectorHub = m_affectorable->getAffectorHub();

            affectorHub->stopAffector( m_id );
            m_id = INVALID_UNIQUE_ID;
        }

        TransformationInterface * transformation = m_transformable->getTransformation();

        transformation->setLocalOrientationX( m_to );
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskTransformationRotateXTime::_onFinally()
    {
        if( m_id != INVALID_UNIQUE_ID )
        {
            const AffectorHubInterfacePtr & affectorHub = m_affectorable->getAffectorHub();

            affectorHub->stopAffector( m_id );
            m_id = INVALID_UNIQUE_ID;
        }

        m_affectorable = nullptr;
        m_transformable = nullptr;

#if defined(MENGINE_DOCUMENT_ENABLE)
        m_doc = nullptr;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
}