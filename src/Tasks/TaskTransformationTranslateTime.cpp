#include "TaskTransformationTranslateTime.h"

#include "Kernel/FactorableUnique.h"

#include "GOAP/NodeInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        class TaskTransformationTranslateTimeAffector
            : public Affector
        {
        public:
            TaskTransformationTranslateTimeAffector( GOAP::NodeInterface * _node, const TransformationPtr & _transformation, const mt::vec3f & _from, const mt::vec3f & _to, float _time )
                : m_node( _node )
                , m_transformation( _transformation )
                , m_from( _from )
                , m_to( _to )
                , m_progress( 0.f )
                , m_time( _time )
            {
            }

            ~TaskTransformationTranslateTimeAffector() override
            {
            }

        public:
            bool _affect( const UpdateContext * _context, float * _used ) override
            {
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
                    mt::linerp_v3( position, m_from, m_to, t_easing );

                    m_transformation->setLocalPosition( position );

                    return false;
                }

                *_used = m_time - m_progress;

                m_transformation->setLocalPosition( m_to );

                return true;
            }

        protected:
            void _complete( bool _isEnd ) override
            {
                MENGINE_UNUSED( _isEnd );

                m_node->complete();
            }

        protected:
            GOAP::NodeInterfacePtr m_node;

            TransformationPtr m_transformation;

            mt::vec3f m_from;
            mt::vec3f m_to;
            float m_progress;
            float m_time;
        };
    }
    //////////////////////////////////////////////////////////////////////////
    TaskTransformationTranslateTime::TaskTransformationTranslateTime( const TransformationPtr & _transformation, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, const mt::vec3f & _to, float _time )
        : m_transformation( _transformation )
        , m_affectorable( _affectorable )
        , m_easing( _easing )
        , m_to( _to )
        , m_time( _time )
        , m_id( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskTransformationTranslateTime::~TaskTransformationTranslateTime()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskTransformationTranslateTime::_onRun( GOAP::NodeInterface * _node )
    {
        const mt::vec3f & position = m_transformation->getLocalPosition();

        AffectorPtr affector = Helper::makeFactorableUnique<Detail::TaskTransformationTranslateTimeAffector>( _node, m_transformation, position, m_to, m_time );

        affector->setEasing( m_easing );

        AFFECTOR_ID id = m_affectorable->addAffector( affector );

        if( id == 0 )
        {
            return true;
        }

        m_id = id;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskTransformationTranslateTime::_onSkip()
    {
        if( m_id != 0 )
        {
            m_affectorable->stopAffector( m_id );
            m_id = 0;
        }

        m_transformation->setLocalPosition( m_to );
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskTransformationTranslateTime::_onFinally()
    {
        if( m_id != 0 )
        {
            m_affectorable->stopAffector( m_id );
            m_id = 0;
        }

        m_affectorable = nullptr;
        m_transformation = nullptr;
    }
}