#include "TaskTransformationRotateX.h"

#include "Interface/TransformationInterface.h"

#include "AffectorTransformationRotate.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Delegate.h"

#include "math/angle.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskTransformationRotateX::TaskTransformationRotateX( GOAP::Allocator * _allocator, const TransformablePtr & _transformable, const AffectorablePtr & _affectorable, float _to, float _speed, const DocumentPtr & _doc )
        : GOAP::TaskInterface( _allocator )
        , m_transformable( _transformable )
        , m_affectorable( _affectorable )
        , m_to( _to )
        , m_speed( _speed )
#ifdef MENGINE_DOCUMENT_ENABLE
        , m_doc( _doc )
#endif
        , m_id( INVALID_UNIQUE_ID )
    {
        MENGINE_UNUSED( _doc );
    }
    //////////////////////////////////////////////////////////////////////////
    TaskTransformationRotateX::~TaskTransformationRotateX()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskTransformationRotateX::_onRun( GOAP::NodeInterface * _node )
    {
        TransformationInterface * transformation = m_transformable->getTransformation();

        float orientationX = transformation->getLocalOrientationX();

        float correct_rotate_from;
        float correct_rotate_to;
        mt::angle_correct_interpolate_from_to( orientationX, m_to, correct_rotate_from, correct_rotate_to );        

        LambdaAffectorTransformationRotate transformCb = Helper::delegate( transformation, &TransformationInterface::setLocalOrientationX );

        AffectorPtr affector = Helper::makeFactorableUnique<AffectorTransformationRotate>( MENGINE_DOCUMENT_VALUE( m_doc, nullptr ), _node, transformCb, correct_rotate_from, correct_rotate_to, m_speed );

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
    void TaskTransformationRotateX::_onSkip()
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
    void TaskTransformationRotateX::_onFinally()
    {
        if( m_id != INVALID_UNIQUE_ID )
        {
            const AffectorHubInterfacePtr & affectorHub = m_affectorable->getAffectorHub();

            affectorHub->stopAffector( m_id );
            m_id = INVALID_UNIQUE_ID;
        }

        m_affectorable = nullptr;
        m_transformable = nullptr;

#ifdef MENGINE_DOCUMENT_ENABLE
        m_doc = nullptr;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
}