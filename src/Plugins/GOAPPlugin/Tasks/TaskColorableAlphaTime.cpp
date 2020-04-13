#include "TaskColorableAlphaTime.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Assertion.h"

#include "GOAP/NodeInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        class TaskColorableAlphaTimeAffector
            : public Affector
        {
        public:
            TaskColorableAlphaTimeAffector( GOAP::NodeInterface * _node, const ColorablePtr & _colorable, float  _from, float _to, float _time )
                : m_node( _node )
                , m_colorable( _colorable )
                , m_from( _from )
                , m_to( _to )
                , m_progress( 0.f )
                , m_time( _time )
            {
            }

            ~TaskColorableAlphaTimeAffector() override
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

                    float result = mt::linerp_f1( m_from, m_to, t_easing );

                    m_colorable->setLocalAlpha( result );

                    return false;
                }

                *_used = m_time - m_progress;

                m_colorable->setLocalAlpha( m_to );

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

            ColorablePtr m_colorable;

            float m_from;
            float m_to;

            float m_progress;
            float m_time;
        };
    }
    //////////////////////////////////////////////////////////////////////////
    TaskColorableAlphaTime::TaskColorableAlphaTime( const ColorablePtr & _colorable, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, float _to, float _time )
        : m_colorable( _colorable )
        , m_affectorable( _affectorable )
        , m_easing( _easing )
        , m_to( _to )
        , m_time( _time )
        , m_id( 0 )
    {
        MENGINE_ASSERTION_FATAL( m_colorable != nullptr );
        MENGINE_ASSERTION_FATAL( m_affectorable != nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    TaskColorableAlphaTime::~TaskColorableAlphaTime()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskColorableAlphaTime::_onRun( GOAP::NodeInterface * _node )
    {
        float alpha = m_colorable->getLocalAlpha();

        AffectorPtr affector = Helper::makeFactorableUnique<Detail::TaskColorableAlphaTimeAffector>( MENGINE_DOCUMENT_FUNCTION, _node, m_colorable, alpha, m_to, m_time );

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
    void TaskColorableAlphaTime::_onSkip()
    {
        if( m_id != 0 )
        {
            m_affectorable->stopAffector( m_id );
            m_id = 0;
        }

        m_colorable->setLocalAlpha( m_to );
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskColorableAlphaTime::_onFinally()
    {
        if( m_id != 0 )
        {
            m_affectorable->stopAffector( m_id );
            m_id = 0;
        }

        m_affectorable = nullptr;
        m_colorable = nullptr;
    }
}