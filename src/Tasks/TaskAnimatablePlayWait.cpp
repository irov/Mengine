#include "TaskAnimatablePlayWait.h"

#include "Kernel/Logger.h"
#include "Kernel/Factorable.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/AnimationEventReceiver.h"

namespace Mengine
{
    namespace Detail
    {
        class TaskAnimatablePlayReceiver
            : public AnimationEventReceiver
            , public Factorable
        {
        public:
            explicit TaskAnimatablePlayReceiver();
            ~TaskAnimatablePlayReceiver() override;

        public:
            void setTask( TaskAnimatablePlayWait * _task );

        protected:
            void onAnimationPlay( uint32_t _enumerator, float _time ) override;
            void onAnimationRestart( uint32_t _enumerator, float _time ) override;
            void onAnimationPause( uint32_t _enumerator ) override;
            void onAnimationResume( uint32_t _enumerator, float _time ) override;
            void onAnimationStop( uint32_t _enumerator ) override;
            void onAnimationEnd( uint32_t _enumerator ) override;
            void onAnimationInterrupt( uint32_t _enumerator ) override;

        protected:
            TaskAnimatablePlayWait * m_task;
        };
        //////////////////////////////////////////////////////////////////////////
        using TaskAnimatablePlayReceiverPtr = IntrusivePtr<TaskAnimatablePlayReceiver>;
        //////////////////////////////////////////////////////////////////////////
        TaskAnimatablePlayReceiver::TaskAnimatablePlayReceiver()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        TaskAnimatablePlayReceiver::~TaskAnimatablePlayReceiver()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        void TaskAnimatablePlayReceiver::setTask( TaskAnimatablePlayWait * _task )
        {
            m_task = _task;
        }
        //////////////////////////////////////////////////////////////////////////
        void TaskAnimatablePlayReceiver::onAnimationPlay( uint32_t _enumerator, float _time )
        {
            MENGINE_UNUSED( _enumerator );
            MENGINE_UNUSED( _time );
        }
        //////////////////////////////////////////////////////////////////////////
        void TaskAnimatablePlayReceiver::onAnimationRestart( uint32_t _enumerator, float _time )
        {
            MENGINE_UNUSED( _enumerator );
            MENGINE_UNUSED( _time );
        }
        //////////////////////////////////////////////////////////////////////////
        void TaskAnimatablePlayReceiver::onAnimationPause( uint32_t _enumerator )
        {
            MENGINE_UNUSED( _enumerator );
        }
        //////////////////////////////////////////////////////////////////////////
        void TaskAnimatablePlayReceiver::onAnimationResume( uint32_t _enumerator, float _time )
        {
            MENGINE_UNUSED( _enumerator );
            MENGINE_UNUSED( _time );
        }
        //////////////////////////////////////////////////////////////////////////
        void TaskAnimatablePlayReceiver::onAnimationStop( uint32_t _enumerator )
        {
            MENGINE_UNUSED( _enumerator );

            m_task->complete();
        }
        //////////////////////////////////////////////////////////////////////////
        void TaskAnimatablePlayReceiver::onAnimationEnd( uint32_t _enumerator )
        {
            MENGINE_UNUSED( _enumerator );

            m_task->complete();
        }
        //////////////////////////////////////////////////////////////////////////
        void TaskAnimatablePlayReceiver::onAnimationInterrupt( uint32_t _enumerator )
        {
            MENGINE_UNUSED( _enumerator );

            m_task->complete();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    TaskAnimatablePlayWait::TaskAnimatablePlayWait( const AnimatablePtr & _animatable, const EventablePtr& _eventable )
        : m_animatable( _animatable )
        , m_eventable( _eventable )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskAnimatablePlayWait::~TaskAnimatablePlayWait()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskAnimatablePlayWait::_onRun()
    {
        AnimationInterface * animation = m_animatable->getAnimation();

        if( animation == nullptr )
        {
            LOGGER_ERROR( "animatable not have animation" );

            return true;
        }

        EventationInterface * eventation = m_eventable->getEventation();

        if( eventation == nullptr )
        {
            LOGGER_ERROR( "eventable not have eventation" );

            return true;
        }

        Detail::TaskAnimatablePlayReceiverPtr receiver = Helper::makeFactorableUnique<Detail::TaskAnimatablePlayReceiver>();
        
        eventation->addEventReceiver( EVENT_ANIMATION_END, receiver );
        eventation->addEventReceiver( EVENT_ANIMATION_STOP, receiver );
        eventation->addEventReceiver( EVENT_ANIMATION_INTERRUPT, receiver );

        receiver->setTask( this );

        animation->play( 0.f );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskAnimatablePlayWait::_onSkipable() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskAnimatablePlayWait::_onSkip()
    {
        AnimationInterface * animation = m_animatable->getAnimation();

        if( animation == nullptr )
        {
            LOGGER_ERROR( "animatable not have animation" );

            return;
        }

        animation->setLastFrame();
        animation->stop();
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskAnimatablePlayWait::_onFinally()
    {
        EventationInterface * eventation = m_eventable->getEventation();

        eventation->removeEventReceiver( EVENT_ANIMATION_END );
        eventation->removeEventReceiver( EVENT_ANIMATION_STOP );
        eventation->removeEventReceiver( EVENT_ANIMATION_INTERRUPT );

        m_animatable = nullptr;
        m_eventable = nullptr;
    }
}