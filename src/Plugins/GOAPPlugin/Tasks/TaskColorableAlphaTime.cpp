#include "TaskColorableAlphaTime.h"

#include "Kernel/FactorableUnique.h"
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
            bool _affect( const UpdateContext * _context, float * const _used ) override
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
    TaskColorableAlphaTime::TaskColorableAlphaTime( GOAP::Allocator * _allocator, const ColorablePtr & _colorable, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, float _to, float _time, const DocumentInterfacePtr & _doc )
        : GOAP::TaskInterface( _allocator )
        , m_colorable( _colorable )
        , m_affectorable( _affectorable )
        , m_easing( _easing )
        , m_to( _to )
        , m_time( _time )
#if defined(MENGINE_DOCUMENT_ENABLE)
        , m_doc( _doc )
#endif
    {
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( m_colorable, "colorable is nullptr" );
        MENGINE_ASSERTION_MEMORY_PANIC( m_affectorable, "affectorable is nullptr" );
    }
    //////////////////////////////////////////////////////////////////////////
    TaskColorableAlphaTime::~TaskColorableAlphaTime()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskColorableAlphaTime::_onRun( GOAP::NodeInterface * _node )
    {
        float alpha = m_colorable->getLocalAlpha();

        AffectorPtr affector = Helper::makeFactorableUnique<Detail::TaskColorableAlphaTimeAffector>( MENGINE_DOCUMENT_VALUE( m_doc, nullptr ), _node, m_colorable, alpha, m_to, m_time );

        affector->setEasing( m_easing );

        const AffectorHubInterfacePtr & affectorHub = m_affectorable->getAffectorHub();

        affectorHub->addAffector( affector );

        m_affector = affector;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskColorableAlphaTime::_onSkip()
    {
        if( m_affector != nullptr )
        {
            m_affector->stop();
            m_affector = nullptr;
        }

        m_colorable->setLocalAlpha( m_to );
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskColorableAlphaTime::_onFinally()
    {
        if( m_affector != nullptr )
        {
            m_affector->stop();
            m_affector = nullptr;
        }

        m_affectorable = nullptr;
        m_colorable = nullptr;

#if defined(MENGINE_DOCUMENT_ENABLE)
        m_doc = nullptr;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
}