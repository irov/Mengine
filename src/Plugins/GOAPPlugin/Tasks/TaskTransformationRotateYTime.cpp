#include "TaskTransformationRotateYTime.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/DocumentHelper.h"

#include "math/angle.h"
#include "math/utils.h"

#include "GOAP/NodeInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        class TaskTransformationRotateYTimeAffector
            : public Affector
        {
        public:
            TaskTransformationRotateYTimeAffector( GOAP::NodeInterface * _node, const TransformationPtr & _transformation, float _from, float _to, float _time )
                : m_node( _node )
                , m_transformation( _transformation )
                , m_from( _from )
                , m_to( _to )
                , m_progress( 0.f )
                , m_time( _time )
            {
            }

            ~TaskTransformationRotateYTimeAffector() override
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

                    float current = mt::linerp_f1( m_from, m_to, t );

                    m_transformation->setLocalOrientationY( current );

                    return true;
                }

                *_used = m_time - m_progress;

                m_transformation->setLocalOrientationY( m_to );

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

            TransformationPtr m_transformation;

            float m_from;
            float m_to;
            float m_progress;
            float m_time;
        };
    }
    //////////////////////////////////////////////////////////////////////////
    TaskTransformationRotateYTime::TaskTransformationRotateYTime( const TransformationPtr & _transformation, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, float _to, float _time )
        : m_transformation( _transformation )
        , m_affectorable( _affectorable )
        , m_easing( _easing )
        , m_to( _to )
        , m_time( _time )
        , m_id( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskTransformationRotateYTime::~TaskTransformationRotateYTime()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskTransformationRotateYTime::_onRun( GOAP::NodeInterface * _node )
    {
        float orientationY = m_transformation->getLocalOrientationY();

        float correct_rotate_from;
        float correct_rotate_to;
        mt::angle_correct_interpolate_from_to( orientationY, m_to, correct_rotate_from, correct_rotate_to );

        AffectorPtr affector = Helper::makeFactorableUnique<Detail::TaskTransformationRotateYTimeAffector>( MENGINE_DOCUMENT_FUNCTION, _node, m_transformation, correct_rotate_from, correct_rotate_to, m_time );

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
    void TaskTransformationRotateYTime::_onSkip()
    {
        if( m_id != 0 )
        {
            m_affectorable->stopAffector( m_id );
            m_id = 0;
        }

        m_transformation->setLocalOrientationY( m_to );
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskTransformationRotateYTime::_onFinally()
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