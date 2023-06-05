#include "TaskTransformationScaleTime.h"

#include "Interface/TransformationInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/DocumentHelper.h"

#include "GOAP/NodeInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        class TaskTransformationScaleTimeAffector
            : public Affector
        {
        public:
            TaskTransformationScaleTimeAffector( GOAP::NodeInterface * _node, const TransformablePtr & _transformable, const mt::vec3f & _from, const mt::vec3f & _to, float _time )
                : m_node( _node )
                , m_transformable( _transformable )
                , m_from( _from )
                , m_to( _to )
                , m_progress( 0.f )
                , m_time( _time )
            {
            }

            ~TaskTransformationScaleTimeAffector() override
            {
            }

        public:
            bool _affect( const UpdateContext * _context, float * const _used ) override
            {
                TransformationInterface * transformation = m_transformable->getTransformation();

                float time = _context->time;

                if( m_progress + time < m_time )
                {
                    *_used = time;

                    m_progress += time;

                    float t = m_progress / m_time;

                    const EasingInterfacePtr & easing = this->getEasing();

                    mt::vec3f scale;
                    if( easing != nullptr )
                    {
                        float t_easing = easing->easing( t );

                        mt::linerp_v3( &scale, m_from, m_to, t_easing );
                    }
                    else
                    {
                        mt::linerp_v3( &scale, m_from, m_to, t );
                    }

                    transformation->setLocalScale( scale );

                    return false;
                }

                *_used = m_time - m_progress;

                transformation->setLocalScale( m_to );

                return true;
            }

        protected:
            void _complete( bool _isEnd ) override
            {
                bool skiped = (_isEnd == false);

                m_node->complete( true, skiped );
            }

        protected:
            GOAP::NodeInterfacePtr m_node;

            TransformablePtr m_transformable;

            mt::vec3f m_from;
            mt::vec3f m_to;
            float m_progress;
            float m_time;
        };
    }
    //////////////////////////////////////////////////////////////////////////
    TaskTransformationScaleTime::TaskTransformationScaleTime( GOAP::Allocator * _allocator, const TransformablePtr & _transformable, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, const mt::vec3f & _to, float _time, uint32_t _flags, const DocumentPtr & _doc )
        : GOAP::TaskInterface( _allocator )
        , m_transformable( _transformable )
        , m_affectorable( _affectorable )
        , m_easing( _easing )
        , m_to( _to )
        , m_time( _time )
        , m_flags( _flags )
#if defined(MENGINE_DOCUMENT_ENABLE)
        , m_doc( _doc )
#endif
        , m_id( 0 )
    {
        MENGINE_UNUSED( _doc );
    }
    //////////////////////////////////////////////////////////////////////////
    TaskTransformationScaleTime::~TaskTransformationScaleTime()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskTransformationScaleTime::_onRun( GOAP::NodeInterface * _node )
    {
        TransformationInterface * transformation = m_transformable->getTransformation();

        const mt::vec3f & scale = transformation->getLocalScale();

        AffectorPtr affector = Helper::makeFactorableUnique<Detail::TaskTransformationScaleTimeAffector>( MENGINE_DOCUMENT_VALUE( m_doc, nullptr ), _node, m_transformable, scale, m_to, m_time );

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
    void TaskTransformationScaleTime::_onSkip()
    {
        if( m_id != 0 )
        {
            const AffectorHubInterfacePtr & affectorHub = m_affectorable->getAffectorHub();

            affectorHub->stopAffector( m_id );
            m_id = 0;
        }

        if( (m_flags & ETASK_FLAG_NOREWIND) == 0 )
        {
            TransformationInterface * transformation = m_transformable->getTransformation();

            transformation->setLocalScale( m_to );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskTransformationScaleTime::_onFinally()
    {
        if( m_id != 0 )
        {
            const AffectorHubInterfacePtr & affectorHub = m_affectorable->getAffectorHub();

            affectorHub->stopAffector( m_id );
            m_id = 0;
        }

        m_affectorable = nullptr;
        m_transformable = nullptr;

#if defined(MENGINE_DOCUMENT_ENABLE)
        m_doc = nullptr;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
}