#include "EngineSource.h"

#include "TaskNodeEnable.h"
#include "TaskNodeDisable.h"
#include "TaskNodeDestroy.h"
#include "TaskDelay.h"
#include "TaskLocalDelay.h"
#include "TaskGlobalDelay.h"
#include "TaskPrint.h"
#include "TaskPickerableMouseButton.h"
#include "TaskPickerableMouseEnter.h"
#include "TaskPickerableMouseLeave.h"
#include "TaskGlobalMouseButton.h"
#include "TaskGlobalMouseMove.h"
#include "TaskGlobalMouseWheel.h"
#include "TaskGlobalKeyPress.h"
#include "TaskTransformationRotateY.h"
#include "TaskTransformationRotateYTime.h"
#include "TaskTransformationScaleTime.h"
#include "TaskTransformationTranslate.h"
#include "TaskTransformationTranslateTime.h"
#include "TaskAnimatablePlay.h"
#include "TaskAnimatablePlayWait.h"
#include "TaskAnimatableStop.h"
#include "TaskAnimatablePause.h"
#include "TaskAnimatableResume.h"
#include "TaskAnimatablePlayReceiver.h"
#include "TaskAnimatableRewind.h"

#include "Config/StdIO.h"

#include "GOAP/SourceProvider.h"
#include "GOAP/Chain.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    EngineSource::EngineSource( const GOAP::SourceProviderInterfacePtr & _provider )
        : GOAP::SourceProxy<EngineSource>( _provider )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EngineSource::~EngineSource()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void EngineSource::addAnimatablePause( const AnimatablePtr & _animatable )
    {
        this->addTask<TaskAnimatablePause>( _animatable );
    }
    //////////////////////////////////////////////////////////////////////////
    void EngineSource::addAnimatablePlay( const AnimatablePtr & _animatable )
    {
        this->addTask<TaskAnimatablePlay>( _animatable );
    }
    //////////////////////////////////////////////////////////////////////////
    void EngineSource::addAnimatablePlayWait( const AnimatablePtr & _animatable, const EventablePtr & _eventable )
    {
        this->addTask<TaskAnimatablePlayWait>( _animatable, _eventable );
    }
    //////////////////////////////////////////////////////////////////////////
    void EngineSource::addAnimatableResume( const AnimatablePtr & _animatable )
    {
        this->addTask<TaskAnimatableResume>( _animatable );
    }
    //////////////////////////////////////////////////////////////////////////
    void EngineSource::addAnimatableRewind( const AnimatablePtr & _animatable )
    {
        this->addTask<TaskAnimatableRewind>( _animatable );
    }
    //////////////////////////////////////////////////////////////////////////
    void EngineSource::addAnimatableStop( const AnimatablePtr & _animatable )
    {
        this->addTask<TaskAnimatableStop>( _animatable );
    }
    //////////////////////////////////////////////////////////////////////////
    void EngineSource::addNodeDestroy( const NodePtr & _node )
    {
        this->addTask<TaskNodeDestroy>( _node );
    }
    //////////////////////////////////////////////////////////////////////////
    void EngineSource::addNodeDisable( const NodePtr & _node )
    {
        this->addTask<TaskNodeDisable>( _node );
    }
    //////////////////////////////////////////////////////////////////////////
    void EngineSource::addNodeEnable( const NodePtr & _node )
    {
        this->addTask<TaskNodeEnable>( _node );
    }
    //////////////////////////////////////////////////////////////////////////
    void EngineSource::addPickerableMouseButton( const PickerablePtr & _pickerable, EMouseCode _code, bool _isDown, bool _isPressed, const LambdaPickerMouseButtonEvent & _filter )
    {
        this->addTask<TaskPickerableMouseButton>( _pickerable, _code, _isDown, _isPressed, _filter );
    }
    //////////////////////////////////////////////////////////////////////////
    void EngineSource::addPickerableMouseEnter( const PickerablePtr & _pickerable, const LambdaPickerMouseEnterEvent & _filter )
    {
        this->addTask<TaskPickerableMouseEnter>( _pickerable, _filter );
    }
    //////////////////////////////////////////////////////////////////////////
    void EngineSource::addPickerableMouseLeave( const PickerablePtr & _pickerable, const LambdaPickerMouseLeaveEvent & _filter )
    {
        this->addTask<TaskPickerableMouseLeave>( _pickerable, _filter );
    }
    //////////////////////////////////////////////////////////////////////////
    void EngineSource::addTransformationRotateY( const TransformationPtr & _transformation, const AffectorablePtr & _affectorable, float _to, float _speed )
    {
        this->addTask<TaskTransformationRotateY>( _transformation, _affectorable, _to, _speed );
    }
    //////////////////////////////////////////////////////////////////////////
    void EngineSource::addTransformationRotateYTime( const TransformationPtr & _transformation, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, float _to, float _time )
    {
        this->addTask<TaskTransformationRotateYTime>( _transformation, _affectorable, _easing, _to, _time );
    }
    //////////////////////////////////////////////////////////////////////////
    void EngineSource::addTransformationScaleTime( const TransformationPtr & _transformation, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, const mt::vec3f & _to, float _time )
    {
        this->addTask<TaskTransformationScaleTime>( _transformation, _affectorable, _easing, _to, _time );
    }
    //////////////////////////////////////////////////////////////////////////
    void EngineSource::addTransformationTranslate( const TransformationPtr & _transformation, const AffectorablePtr & _affectorable, const mt::vec3f & _to, float _speed )
    {
        this->addTask<TaskTransformationTranslate>( _transformation, _affectorable, _to, _speed );
    }
    //////////////////////////////////////////////////////////////////////////
    void EngineSource::addTransformationTranslateTime( const TransformationPtr & _transformation, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, const mt::vec3f & _to, float _time )
    {
        this->addTask<TaskTransformationTranslateTime>( _transformation, _affectorable, _easing, _to, _time );
    }
    //////////////////////////////////////////////////////////////////////////
    void EngineSource::addPrint( const Char * _format, ... )
    {
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _format );

        this->addTask<TaskPrint>( _format, args );

        MENGINE_VA_LIST_END( args );
    }
    //////////////////////////////////////////////////////////////////////////
    void EngineSource::addDelay( const SchedulerInterfacePtr & _scheduler, float _time )
    {
        this->addTask<TaskDelay>( _scheduler, _time );
    }
    //////////////////////////////////////////////////////////////////////////
    void EngineSource::addGlobalDelay( float _time )
    {
        this->addTask<TaskGlobalDelay>( _time );
    }
    //////////////////////////////////////////////////////////////////////////
    void EngineSource::addLocalDelay( float _time )
    {
        this->addTask<TaskLocalDelay>( _time );
    }
    //////////////////////////////////////////////////////////////////////////
    void EngineSource::addGlobalKeyPress( EKeyCode _code, bool _isDown, const LambdaInputKeyEvent & _filter )
    {
        this->addTask<TaskGlobalKeyPress>( _code, _isDown, _filter );
    }
    //////////////////////////////////////////////////////////////////////////    
    void EngineSource::addGlobalMouseButton( EMouseCode _code, bool _isDown, const LambdaInputMouseButtonEvent & _filter )
    {
        this->addTask<TaskGlobalMouseButton>( _code, _isDown, _filter );
    }
    //////////////////////////////////////////////////////////////////////////
    void EngineSource::addGlobalMouseMove( const LambdaInputMouseMoveEvent & _filter )
    {
        this->addTask<TaskGlobalMouseMove>( _filter );
    }
    //////////////////////////////////////////////////////////////////////////
    void EngineSource::addGlobalMouseWheel( const LambdaInputMouseWheelEvent & _filter )
    {
        this->addTask<TaskGlobalMouseWheel>( _filter );
    }
    //////////////////////////////////////////////////////////////////////////
    GOAP::SourceInterfacePtr EngineSource::_makeSource()
    {
        return Helper::makeEngineSource();
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        EngineSourcePtr makeEngineSource()
        {
            GOAP::SourceProviderPtr provider = GOAP::Helper::makeSourceProvider();

            return EngineSourcePtr::from( new EngineSource( provider ) );
        }
        //////////////////////////////////////////////////////////////////////////
        GOAP::ChainInterfacePtr makeEngineChain( const EngineSourcePtr & _source )
        {
            GOAP::ChainPtr chain = GOAP::Helper::makeChain( _source );

            return chain;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}