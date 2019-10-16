#include "TaskTransformationScaleTime.h"

#include "Kernel/FactorableUnique.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        class TaskTransformationScaleTimeAffector
            : public Affector
        {
        public:
            TaskTransformationScaleTimeAffector( const TaskTransformationScaleTimePtr & _task, const TransformationPtr & _transformation, const mt::vec3f & _from, const mt::vec3f & _to, float _time )
                : m_task( _task )
                , m_transformation( _transformation )
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
            bool _affect( const UpdateContext * _context, float * _used ) override
            {
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

                        mt::linerp_v3( scale, m_from, m_to, t_easing );
                    }
                    else
                    {
                        mt::linerp_v3( scale, m_from, m_to, t );
                    }

                    m_transformation->setLocalScale( scale );

                    return false;
                }

                *_used = m_time - m_progress;

                m_transformation->setLocalScale( m_to );

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
            TaskTransformationScaleTimePtr m_task;

            TransformationPtr m_transformation;

            mt::vec3f m_from;
            mt::vec3f m_to;
            float m_progress;
            float m_time;
        };
    }
    //////////////////////////////////////////////////////////////////////////
    TaskTransformationScaleTime::TaskTransformationScaleTime( const TransformationPtr & _transformation, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, const mt::vec3f & _to, float _time, uint32_t _flags )
        : m_transformation( _transformation )
        , m_affectorable( _affectorable )
        , m_easing( _easing )
        , m_to( _to )
        , m_time( _time )
        , m_id( 0 )
        , m_flags( _flags )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskTransformationScaleTime::~TaskTransformationScaleTime()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskTransformationScaleTime::_onRun()
    {
        const mt::vec3f & scale = m_transformation->getLocalScale();

        AffectorPtr affector = Helper::makeFactorableUnique<Detail::TaskTransformationScaleTimeAffector>( TaskTransformationScaleTimePtr::from( this ), m_transformation, scale, m_to, m_time );

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
    void TaskTransformationScaleTime::_onSkip()
    {
        if( m_id != 0 )
        {
            m_affectorable->stopAffector( m_id );
            m_id = 0;
        }

        if( (m_flags & ETASK_FLAG_NOREWIND) == 0 )
        {
            m_transformation->setLocalScale( m_to );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskTransformationScaleTime::_onFinally()
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