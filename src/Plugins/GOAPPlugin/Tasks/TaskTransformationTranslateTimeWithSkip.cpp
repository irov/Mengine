#include "TaskTransformationTranslateTimeWithSkip.h"

#include "Interface/TransformationInterface.h"

#include "Kernel/FactorableUnique.h"

#include "GOAP/NodeInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        class TaskTransformationTranslateTimeWithSkipAffector
            : public Affector
        {
        public:
            TaskTransformationTranslateTimeWithSkipAffector( GOAP::NodeInterface * _node, const TransformablePtr & _transformable, const mt::vec3f & _from, const mt::vec3f & _to, float _time )
                : m_node( _node )
                , m_transformable( _transformable )
                , m_from( _from )
                , m_to( _to )
                , m_progress( 0.f )
                , m_time( _time )
            {
            }

            ~TaskTransformationTranslateTimeWithSkipAffector() override
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

                    float t_easing = 0.f;

                    if( easing == nullptr )
                    {
                        t_easing = t;
                    }
                    else
                    {
                        t_easing = easing->easing( t );
                    }

                    mt::vec3f position;
                    mt::linerp_v3( &position, m_from, m_to, t_easing );

                    transformation->setLocalPosition( position );

                    return false;
                }

                *_used = m_time - m_progress;

                transformation->setLocalPosition( m_to );

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
    TaskTransformationTranslateTimeWithSkip::TaskTransformationTranslateTimeWithSkip( GOAP::Allocator * _allocator, const TransformablePtr & _transformable, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, const mt::vec3f & _to, float _time, const DocumentInterfacePtr & _doc )
        : GOAP::TaskInterface( _allocator )
        , m_transformable( _transformable )
        , m_affectorable( _affectorable )
        , m_easing( _easing )
        , m_to( _to )
        , m_time( _time )
#if defined(MENGINE_DOCUMENT_ENABLE)
        , m_doc( _doc )
#endif
        , m_id( 0 )
    {
        MENGINE_UNUSED( _doc );
    }
    //////////////////////////////////////////////////////////////////////////
    TaskTransformationTranslateTimeWithSkip::~TaskTransformationTranslateTimeWithSkip()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskTransformationTranslateTimeWithSkip::_onRun( GOAP::NodeInterface * _node )
    {
        TransformationInterface * transformation = m_transformable->getTransformation();

        const mt::vec3f & position = transformation->getLocalPosition();

        AffectorPtr affector = Helper::makeFactorableUnique<Detail::TaskTransformationTranslateTimeWithSkipAffector>( MENGINE_DOCUMENT_VALUE( m_doc, nullptr ), _node, m_transformable, position, m_to, m_time );

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
    void TaskTransformationTranslateTimeWithSkip::_onSkip()
    {
        if( m_id != 0 )
        {
            const AffectorHubInterfacePtr & affectorHub = m_affectorable->getAffectorHub();

            affectorHub->stopAffector( m_id );
            m_id = 0;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskTransformationTranslateTimeWithSkip::_onFinally()
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