#include "TaskTransformationRotateY.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/DocumentHelper.h"

#include "math/angle.h"

#include "GOAP/NodeInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        class TaskTransformationRotateYAffector
            : public Affector
        {
        public:
            TaskTransformationRotateYAffector( GOAP::NodeInterface * _node, const TransformationPtr & _transformation, float _from, float _to, float _speed )
                : m_node( _node )
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
            bool _affect( const UpdateContext * _context, float * const _used ) override
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
                bool skiped = (_isEnd == false);

                m_node->complete( true, skiped );
            }

        protected:
            GOAP::NodeInterfacePtr m_node;

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
    bool TaskTransformationRotateY::_onRun( GOAP::NodeInterface * _node )
    {
        float orientationY = m_transformation->getLocalOrientationY();

        float correct_rotate_from;
        float correct_rotate_to;
        mt::angle_correct_interpolate_from_to( orientationY, m_to, correct_rotate_from, correct_rotate_to );

        AffectorPtr affector = Helper::makeFactorableUnique<Detail::TaskTransformationRotateYAffector>( MENGINE_DOCUMENT_FUNCTION, _node, m_transformation, correct_rotate_from, correct_rotate_to, m_speed );

        const AffectorHubInterfacePtr & affectorHub = m_affectorable->getAffectorHub();

        AFFECTOR_ID id = affectorHub->addAffector( affector );

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
            const AffectorHubInterfacePtr & affectorHub = m_affectorable->getAffectorHub();

            affectorHub->stopAffector( m_id );
            m_id = 0;
        }

        m_transformation->setLocalOrientationY( m_to );
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskTransformationRotateY::_onFinally()
    {
        if( m_id != 0 )
        {
            const AffectorHubInterfacePtr & affectorHub = m_affectorable->getAffectorHub();

            affectorHub->stopAffector( m_id );
            m_id = 0;
        }

        m_affectorable = nullptr;
        m_transformation = nullptr;
    }
}