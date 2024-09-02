#include "TaskTransformationTranslate.h"

#include "Interface/TransformationInterface.h"

#include "Kernel/FactorableUnique.h"

#include "GOAP/NodeInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        class TaskTransformationTranslateAffector
            : public Affector
        {
        public:
            TaskTransformationTranslateAffector( GOAP::NodeInterface * _node, const TransformablePtr & _transformable, const mt::vec3f & _delta, const mt::vec3f & _to, float _speed )
                : m_node( _node )
                , m_transformable( _transformable )
                , m_delta( _delta )
                , m_to( _to )
                , m_speed( _speed )
            {
            }

            ~TaskTransformationTranslateAffector() override
            {
            }

        public:
            bool _affect( const UpdateContext * _context, float * const _used ) override
            {
                TransformationInterface * transformation = m_transformable->getTransformation();

                float time = _context->time;

                mt::vec3f translate = m_delta * time;

                const mt::vec3f & position = transformation->getLocalPosition();

                float way_sqrlength = mt::sqrlength_v3_v3( position, m_to );
                float translate_sqrlength = mt::sqrlength_v3( translate );

                if( translate_sqrlength < way_sqrlength )
                {
                    *_used = time;

                    transformation->translate( translate );

                    return true;
                }

                *_used = (way_sqrlength - translate_sqrlength) / translate_sqrlength;

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

            mt::vec3f m_delta;
            mt::vec3f m_to;
            float m_speed;
        };
    }
    //////////////////////////////////////////////////////////////////////////
    TaskTransformationTranslate::TaskTransformationTranslate( GOAP::Allocator * _allocator, const TransformablePtr & _transformable, const AffectorablePtr & _affectorable, const mt::vec3f & _to, float _speed, const DocumentInterfacePtr & _doc )
        : GOAP::TaskInterface( _allocator )
        , m_transformable( _transformable )
        , m_affectorable( _affectorable )
        , m_to( _to )
        , m_speed( _speed )
#if defined(MENGINE_DOCUMENT_ENABLE)
        , m_doc( _doc )
#endif
        , m_id( 0 )
    {
        MENGINE_UNUSED( _doc );
    }
    //////////////////////////////////////////////////////////////////////////
    TaskTransformationTranslate::~TaskTransformationTranslate()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskTransformationTranslate::_onRun( GOAP::NodeInterface * _node )
    {
        TransformationInterface * transformation = m_transformable->getTransformation();

        const mt::vec3f & position = transformation->getLocalPosition();

        mt::vec3f dir;
        mt::dir_v3_v3( &dir, position, m_to );

        mt::vec3f delta = dir * m_speed;

        AffectorPtr affector = Helper::makeFactorableUnique<Detail::TaskTransformationTranslateAffector>( MENGINE_DOCUMENT_VALUE( m_doc, nullptr ), _node, m_transformable, delta, m_to, m_speed );

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
    void TaskTransformationTranslate::_onSkip()
    {
        if( m_id != 0 )
        {
            const AffectorHubInterfacePtr & affectorHub = m_affectorable->getAffectorHub();

            affectorHub->stopAffector( m_id );
            m_id = 0;
        }

        TransformationInterface * transformation = m_transformable->getTransformation();

        transformation->setLocalPosition( m_to );
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskTransformationTranslate::_onFinally()
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