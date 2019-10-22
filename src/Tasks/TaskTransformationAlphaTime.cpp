#include "TaskTransformationAlphaTime.h"

#include "Kernel/FactorableUnique.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        class TaskTransformationAlphaTimeAffector
            : public Affector
        {
        public:
            TaskTransformationAlphaTimeAffector( const TaskTransformationAlphaTimePtr & _task, const RenderInterfacePtr & _renderable, float _from, float _to, float _time )
                : m_task( _task )
                , m_renderable( _renderable )
                , m_from( _from )
                , m_to( _to )
                , m_progress( 0.f )
                , m_time( _time )
            {
            }

            ~TaskTransformationAlphaTimeAffector() override
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

                    float t_easing = easing->easing( t );
                    MENGINE_UNUSED( t_easing );

                    m_renderable->setLocalAlpha( m_from );

                    return false;
                }

                *_used = m_time - m_progress;

                m_renderable->setLocalAlpha( m_to );

                return true;
            }

        protected:
            void _complete( bool _isEnd ) override
            {
                if( _isEnd == true )
                {
                    m_task->complete();
                }
                else
                {
                    m_task->skip();
                }
            }

        protected:
            TaskTransformationAlphaTimePtr m_task;

            RenderInterfacePtr m_renderable;

            float m_from;
            float m_to;

            float m_progress;
            float m_time;
        };
    }
    //////////////////////////////////////////////////////////////////////////
    TaskTransformationAlphaTime::TaskTransformationAlphaTime( const RenderInterfacePtr & _renderable, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, float  _to, float _time )
        : m_renderable( _renderable )
        , m_affectorable( _affectorable )
        , m_easing( _easing )
        , m_to( _to )
        , m_time( _time )
        , m_id( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskTransformationAlphaTime::~TaskTransformationAlphaTime()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskTransformationAlphaTime::_onRun()
    {
        const float alpha = m_renderable->getLocalAlpha();

        AffectorPtr affector = Helper::makeFactorableUnique<Detail::TaskTransformationAlphaTimeAffector>( TaskTransformationAlphaTimePtr::from( this ), m_renderable, alpha, m_to, m_time );

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
    void TaskTransformationAlphaTime::_onSkip()
    {
        if( m_id != 0 )
        {
            m_affectorable->stopAffector( m_id );
            m_id = 0;
        }

        m_renderable->setLocalAlpha( m_to );
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskTransformationAlphaTime::_onFinally()
    {
        if( m_id != 0 )
        {
            m_affectorable->stopAffector( m_id );
            m_id = 0;
        }

        m_affectorable = nullptr;
        m_renderable = nullptr;
    }
}