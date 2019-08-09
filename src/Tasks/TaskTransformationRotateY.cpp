#include "TaskTransformationRotateY.h"

#include "Kernel/FactorableUnique.h"

#include "math/angle.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        class TaskTransformationRotateYAffector
            : public Affector
        {
        public:
            TaskTransformationRotateYAffector( TaskTransformationRotateY * _task, const TransformationPtr & _transformation, float _from, float _to, float _speed )
                : m_task( _task )
                , m_transformation( _transformation )
                , m_from( _from )
                , m_to( _to )
                , m_progress( _from )
                , m_speed( _speed )
            {
            }

            ~TaskTransformationRotateYAffector() override
            {
            }

        public:
            bool _affect( const UpdateContext * _context, float * _used ) override
            {
                float time = _context->time;

                float deltha = m_to - m_from;

                float add = deltha * m_speed * time;

                if( m_progress + add < m_to )
                {
                    m_progress += add;

                    *_used = time;

                    m_transformation->setLocalOrientationY( m_progress );

                    return false;
                }

                *_used = time * (m_to - m_progress) / add;

                m_progress = m_to;

                m_transformation->setLocalOrientationY( m_progress );

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
            TaskTransformationRotateY * m_task;

            TransformationPtr m_transformation;

            float m_from;
            float m_to;
            float m_progress;
            float m_speed;
        };
    }
    //////////////////////////////////////////////////////////////////////////
    TaskTransformationRotateY::TaskTransformationRotateY( const TransformationPtr & _transformation, const AffectorablePtr & _affectorable, float _to, float _speed )
        : m_transformation( _transformation )
        , m_affectorable( _affectorable )
        , m_to( _to )
        , m_speed( _speed )
        , m_id( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskTransformationRotateY::~TaskTransformationRotateY()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskTransformationRotateY::_onRun()
    {
        float orientationY = m_transformation->getLocalOrientationY();

        float correct_rotate_from;
        float correct_rotate_to;
        mt::angle_correct_interpolate_from_to( orientationY, m_to, correct_rotate_from, correct_rotate_to );

        AffectorPtr affector = Helper::makeFactorableUnique<Detail::TaskTransformationRotateYAffector>( this, m_transformation, correct_rotate_from, correct_rotate_to, m_speed );

        AFFECTOR_ID id = m_affectorable->addAffector( affector );

        if( id == 0 )
        {
            return true;
        }

        m_id = id;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskTransformationRotateY::_onSkip()
    {
        if( m_id != 0 )
        {
            m_affectorable->stopAffector( m_id );
            m_id = 0;
        }

        m_transformation->setLocalOrientationY( m_to );
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskTransformationRotateY::_onFinally()
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