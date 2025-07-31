#include "TaskTransformationRotateZTime.h"

#include "Interface/TransformationInterface.h"

#include "AffectorTransformationRotateTime.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/Delegate.h"

#include "math/angle.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskTransformationRotateZTime::TaskTransformationRotateZTime( GOAP::Allocator * _allocator, const TransformablePtr & _transformable, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, float _to, float _time, ETransformationRotateMode _mode, const DocumentInterfacePtr & _doc )
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
    {
        MENGINE_UNUSED( _doc );
    }
    //////////////////////////////////////////////////////////////////////////
    TaskTransformationRotateZTime::~TaskTransformationRotateZTime()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskTransformationRotateZTime::_onRun( GOAP::NodeInterface * _node )
    {
        TransformationInterface * transformation = m_transformable->getTransformation();

        float orientationZ = transformation->getLocalOrientationZ();

        LambdaAffectorTransformationRotateTime transformCb = Helper::delegate( transformation, &TransformationInterface::setLocalOrientationZ );

        AffectorPtr affector = Helper::makeFactorableUnique<AffectorTransformationRotateTime>( MENGINE_DOCUMENT_VALUE( m_doc, nullptr ), _node, transformCb, orientationZ, m_to, m_time, m_mode );

        affector->setEasing( m_easing );

        const AffectorHubInterfacePtr & affectorHub = m_affectorable->getAffectorHub();

        affectorHub->addAffector( affector );

        m_affector = affector;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskTransformationRotateZTime::_onSkip()
    {
        if( m_affector != nullptr )
        {
            m_affector->stop();
            m_affector = nullptr;
        }

        TransformationInterface * transformation = m_transformable->getTransformation();

        transformation->setLocalOrientationZ( m_to );
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskTransformationRotateZTime::_onFinally()
    {
        if( m_affector != nullptr )
        {
            m_affector->stop();
            m_affector = nullptr;
        }

        m_affectorable = nullptr;
        m_transformable = nullptr;

#if defined(MENGINE_DOCUMENT_ENABLE)
        m_doc = nullptr;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
}