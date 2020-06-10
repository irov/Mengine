#include "GOAPCook.h"

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
#include "TaskTransformationRotateX.h"
#include "TaskTransformationRotateY.h"
#include "TaskTransformationRotateZ.h"
#include "TaskTransformationRotateXTime.h"
#include "TaskTransformationRotateYTime.h"
#include "TaskTransformationRotateZTime.h"
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
#include "TaskColorableAlphaTime.h"

#include "Config/StdIO.h"

#include "GOAP/SourceProviderInterface.h"
#include "GOAP/ChainInterface.h"

namespace Mengine
{
    namespace Cook
    {
        //////////////////////////////////////////////////////////////////////////
        void addAnimatablePause( const GOAP::SourceInterfacePtr & _source, const AnimatablePtr & _animatable )
        {
            Cook::addTask<TaskAnimatablePause>( _source, _animatable );
        }
        //////////////////////////////////////////////////////////////////////////
        void addAnimatablePlay( const GOAP::SourceInterfacePtr & _source, const AnimatablePtr & _animatable )
        {
            Cook::addTask<TaskAnimatablePlay>( _source, _animatable );
        }
        //////////////////////////////////////////////////////////////////////////
        void addAnimatablePlayWait( const GOAP::SourceInterfacePtr & _source, const AnimatablePtr & _animatable, const EventablePtr & _eventable )
        {
            Cook::addTask<TaskAnimatablePlayWait>( _source, _animatable, _eventable );
        }
        //////////////////////////////////////////////////////////////////////////
        void addAnimatableResume( const GOAP::SourceInterfacePtr & _source, const AnimatablePtr & _animatable )
        {
            Cook::addTask<TaskAnimatableResume>( _source, _animatable );
        }
        //////////////////////////////////////////////////////////////////////////
        void addAnimatableRewind( const GOAP::SourceInterfacePtr & _source, const AnimatablePtr & _animatable )
        {
            Cook::addTask<TaskAnimatableRewind>( _source, _animatable );
        }
        //////////////////////////////////////////////////////////////////////////
        void addAnimatableStop( const GOAP::SourceInterfacePtr & _source, const AnimatablePtr & _animatable )
        {
            Cook::addTask<TaskAnimatableStop>( _source, _animatable );
        }
        //////////////////////////////////////////////////////////////////////////
        void addNodeDestroy( const GOAP::SourceInterfacePtr & _source, const NodePtr & _node )
        {
            Cook::addTask<TaskNodeDestroy>( _source, _node );
        }
        //////////////////////////////////////////////////////////////////////////
        void addNodeDisable( const GOAP::SourceInterfacePtr & _source, const NodePtr & _node )
        {
            Cook::addTask<TaskNodeDisable>( _source, _node );
        }
        //////////////////////////////////////////////////////////////////////////
        void addNodeEnable( const GOAP::SourceInterfacePtr & _source, const NodePtr & _node )
        {
            Cook::addTask<TaskNodeEnable>( _source, _node );
        }
        //////////////////////////////////////////////////////////////////////////
        void addPickerableMouseButton( const GOAP::SourceInterfacePtr & _source, const PickerablePtr & _pickerable, EMouseCode _code, bool _isDown, bool _isPressed, const LambdaPickerMouseButtonEvent & _filter )
        {
            Cook::addTask<TaskPickerableMouseButton>( _source, _pickerable, _code, _isDown, _isPressed, _filter );
        }
        //////////////////////////////////////////////////////////////////////////
        void addPickerableMouseEnter( const GOAP::SourceInterfacePtr & _source, const PickerablePtr & _pickerable, const LambdaPickerMouseEnterEvent & _filter )
        {
            Cook::addTask<TaskPickerableMouseEnter>( _source, _pickerable, _filter );
        }
        //////////////////////////////////////////////////////////////////////////
        void addPickerableMouseLeave( const GOAP::SourceInterfacePtr & _source, const PickerablePtr & _pickerable, const LambdaPickerMouseLeaveEvent & _filter )
        {
            Cook::addTask<TaskPickerableMouseLeave>( _source, _pickerable, _filter );
        }
        //////////////////////////////////////////////////////////////////////////
        void addTransformationRotateX( const GOAP::SourceInterfacePtr & _source, const TransformationPtr & _transformation, const AffectorablePtr & _affectorable, float _to, float _speed )
        {
            Cook::addTask<TaskTransformationRotateX>( _source, _transformation, _affectorable, _to, _speed );
        }
        //////////////////////////////////////////////////////////////////////////
        void addTransformationRotateY( const GOAP::SourceInterfacePtr & _source, const TransformationPtr & _transformation, const AffectorablePtr & _affectorable, float _to, float _speed )
        {
            Cook::addTask<TaskTransformationRotateY>( _source, _transformation, _affectorable, _to, _speed );
        }
        //////////////////////////////////////////////////////////////////////////
        void addTransformationRotateZ( const GOAP::SourceInterfacePtr & _source, const TransformationPtr & _transformation, const AffectorablePtr & _affectorable, float _to, float _speed )
        {
            Cook::addTask<TaskTransformationRotateZ>( _source, _transformation, _affectorable, _to, _speed );
        }
        //////////////////////////////////////////////////////////////////////////
        void addTransformationRotateXTime( const GOAP::SourceInterfacePtr & _source, const TransformationPtr & _transformation, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, float _to, float _time, ETransformationRotateMode _mode )
        {
            Cook::addTask<TaskTransformationRotateXTime>( _source, _transformation, _affectorable, _easing, _to, _time, _mode );
        }
        //////////////////////////////////////////////////////////////////////////
        void addTransformationRotateYTime( const GOAP::SourceInterfacePtr & _source, const TransformationPtr & _transformation, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, float _to, float _time, ETransformationRotateMode _mode )
        {
            Cook::addTask<TaskTransformationRotateYTime>( _source, _transformation, _affectorable, _easing, _to, _time, _mode );
        }
        //////////////////////////////////////////////////////////////////////////
        void addTransformationRotateZTime( const GOAP::SourceInterfacePtr & _source, const TransformationPtr & _transformation, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, float _to, float _time, ETransformationRotateMode _mode )
        {
            Cook::addTask<TaskTransformationRotateZTime>( _source, _transformation, _affectorable, _easing, _to, _time, _mode );
        }
        //////////////////////////////////////////////////////////////////////////
        void addTransformationScaleTime( const GOAP::SourceInterfacePtr & _source, const TransformationPtr & _transformation, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, const mt::vec3f & _to, float _time, uint32_t _flags )
        {
            Cook::addTask<TaskTransformationScaleTime>( _source, _transformation, _affectorable, _easing, _to, _time, _flags );
        }
        //////////////////////////////////////////////////////////////////////////
        void addTransformationTranslate( const GOAP::SourceInterfacePtr & _source, const TransformationPtr & _transformation, const AffectorablePtr & _affectorable, const mt::vec3f & _to, float _speed )
        {
            Cook::addTask<TaskTransformationTranslate>( _source, _transformation, _affectorable, _to, _speed );
        }
        //////////////////////////////////////////////////////////////////////////
        void addTransformationTranslateTime( const GOAP::SourceInterfacePtr & _source, const TransformationPtr & _transformation, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, const mt::vec3f & _to, float _time )
        {
            Cook::addTask<TaskTransformationTranslateTime>( _source, _transformation, _affectorable, _easing, _to, _time );
        }
        //////////////////////////////////////////////////////////////////////////
        void addColorableAlphaTime( const GOAP::SourceInterfacePtr & _source, const ColorablePtr & _transformation, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, float _to, float _time )
        {
            Cook::addTask<TaskColorableAlphaTime>( _source, _transformation, _affectorable, _easing, _to, _time );
        }
        //////////////////////////////////////////////////////////////////////////
        void addPrint( const GOAP::SourceInterfacePtr & _source, const Char * _format, ... )
        {
            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _format );

            Cook::addTask<TaskPrint>( _source, _format, args );

            MENGINE_VA_LIST_END( args );
        }
        //////////////////////////////////////////////////////////////////////////
        void addDelay( const GOAP::SourceInterfacePtr & _source, const SchedulerInterfacePtr & _scheduler, float _time )
        {
            Cook::addTask<TaskDelay>( _source, _scheduler, _time );
        }
        //////////////////////////////////////////////////////////////////////////
        void addGlobalDelay( const GOAP::SourceInterfacePtr & _source, float _time )
        {
            Cook::addTask<TaskGlobalDelay>( _source, _time );
        }
        //////////////////////////////////////////////////////////////////////////
        void addLocalDelay( const GOAP::SourceInterfacePtr & _source, float _time )
        {
            Cook::addTask<TaskLocalDelay>( _source, _time );
        }
        //////////////////////////////////////////////////////////////////////////
        void addGlobalKeyPress( const GOAP::SourceInterfacePtr & _source, EKeyCode _code, bool _isDown, const LambdaInputKeyEvent & _filter )
        {
            Cook::addTask<TaskGlobalKeyPress>( _source, _code, _isDown, _filter );
        }
        //////////////////////////////////////////////////////////////////////////    
        void addGlobalMouseButton( const GOAP::SourceInterfacePtr & _source, EMouseCode _code, bool _isDown, const LambdaInputMouseButtonEvent & _filter )
        {
            Cook::addTask<TaskGlobalMouseButton>( _source, _code, _isDown, _filter );
        }
        //////////////////////////////////////////////////////////////////////////
        void addGlobalMouseMove( const GOAP::SourceInterfacePtr & _source, const LambdaInputMouseMoveEvent & _filter )
        {
            Cook::addTask<TaskGlobalMouseMove>( _source, _filter );
        }
        //////////////////////////////////////////////////////////////////////////
        void addGlobalMouseWheel( const GOAP::SourceInterfacePtr & _source, const LambdaInputMouseWheelEvent & _filter )
        {
            Cook::addTask<TaskGlobalMouseWheel>( _source, _filter );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}