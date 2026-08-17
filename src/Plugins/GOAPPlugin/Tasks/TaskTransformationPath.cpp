#include "TaskTransformationPath.h"

#include "Interface/TransformationInterface.h"

#include "Kernel/FactorableUnique.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskTransformationPath::TaskTransformationPath( GOAP::Allocator * _allocator, const TransformablePtr & _transformable, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, const Vector<mt::vec3f> & _points, float _duration, ETransformationPathMode _mode, const DocumentInterfacePtr & _doc )
        : GOAP::TaskInterface( _allocator )
        , m_transformable( _transformable )
        , m_affectorable( _affectorable )
        , m_easing( _easing )
        , m_points( _points )
        , m_duration( _duration )
        , m_mode( _mode )
#if defined(MENGINE_DOCUMENT_ENABLE)
        , m_doc( _doc )
#endif
    {
        MENGINE_UNUSED( _doc );
    }
    //////////////////////////////////////////////////////////////////////////
    TaskTransformationPath::~TaskTransformationPath()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskTransformationPath::_onRun( GOAP::NodeInterface * _node )
    {
        if( m_points.empty() == true )
        {
            return true;
        }

        AffectorTransformationPathPtr affector = Helper::makeFactorableUnique<AffectorTransformationPath>( MENGINE_DOCUMENT_VALUE( m_doc, nullptr ), _node, m_transformable, m_points, m_duration, m_mode );

        affector->setEasing( m_easing );
        m_affectorable->addAffector( affector );

        m_affector = affector;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskTransformationPath::_onSkip()
    {
        if( m_affector != nullptr )
        {
            m_affector->stop();
            m_affector = nullptr;
        }

        if( m_points.empty() == false )
        {
            m_transformable->getTransformation()->setLocalPosition( m_points.back() );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskTransformationPath::_onFinally()
    {
        if( m_affector != nullptr )
        {
            m_affector->stop();
            m_affector = nullptr;
        }

        m_points.clear();
        m_easing = nullptr;
        m_affectorable = nullptr;
        m_transformable = nullptr;

#if defined(MENGINE_DOCUMENT_ENABLE)
        m_doc = nullptr;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
}
