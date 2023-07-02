#include "GOAPCook.h"

#include "TaskNodeEnable.h"
#include "TaskNodeDisable.h"
#include "TaskNodeDestroy.h"
#include "TaskDelay.h"
#include "TaskLocalDelay.h"
#include "TaskGlobalDelay.h"
#include "TaskPrint.h"
#include "TaskReleasePrint.h"
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
#include "TaskTransformationTranslateTimeWithSkip.h"
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
        void addAnimatablePlayWait( const GOAP::SourceInterfacePtr & _source, const AnimatablePtr & _animatable, const EventablePtr & _eventable, const DocumentInterfacePtr & _doc )
        {
            Cook::addTask<TaskAnimatablePlayWait>( _source, _animatable, _eventable, _doc );
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
        void addPickerableMouseButton( const GOAP::SourceInterfacePtr & _source, const PickerablePtr & _pickerable, EMouseButtonCode _button, bool _isDown, bool _isPressed, const LambdaPickerMouseButtonEvent & _filter, const DocumentInterfacePtr & _doc )
        {
            Cook::addTask<TaskPickerableMouseButton>( _source, _pickerable, _button, _isDown, _isPressed, _filter, _doc );
        }
        //////////////////////////////////////////////////////////////////////////
        void addPickerableMouseEnter( const GOAP::SourceInterfacePtr & _source, const PickerablePtr & _pickerable, const LambdaPickerMouseEnterEvent & _filter, const DocumentInterfacePtr & _doc )
        {
            Cook::addTask<TaskPickerableMouseEnter>( _source, _pickerable, _filter, _doc );
        }
        //////////////////////////////////////////////////////////////////////////
        void addPickerableMouseLeave( const GOAP::SourceInterfacePtr & _source, const PickerablePtr & _pickerable, const LambdaPickerMouseLeaveEvent & _filter, const DocumentInterfacePtr & _doc )
        {
            Cook::addTask<TaskPickerableMouseLeave>( _source, _pickerable, _filter, _doc );
        }
        //////////////////////////////////////////////////////////////////////////
        void addTransformationRotateX( const GOAP::SourceInterfacePtr & _source, const TransformablePtr & _transformation, const AffectorablePtr & _affectorable, float _to, float _speed, const DocumentInterfacePtr & _doc )
        {
            Cook::addTask<TaskTransformationRotateX>( _source, _transformation, _affectorable, _to, _speed, _doc );
        }
        //////////////////////////////////////////////////////////////////////////
        void addTransformationRotateY( const GOAP::SourceInterfacePtr & _source, const TransformablePtr & _transformation, const AffectorablePtr & _affectorable, float _to, float _speed, const DocumentInterfacePtr & _doc )
        {
            Cook::addTask<TaskTransformationRotateY>( _source, _transformation, _affectorable, _to, _speed, _doc );
        }
        //////////////////////////////////////////////////////////////////////////
        void addTransformationRotateZ( const GOAP::SourceInterfacePtr & _source, const TransformablePtr & _transformation, const AffectorablePtr & _affectorable, float _to, float _speed, const DocumentInterfacePtr & _doc )
        {
            Cook::addTask<TaskTransformationRotateZ>( _source, _transformation, _affectorable, _to, _speed, _doc );
        }
        //////////////////////////////////////////////////////////////////////////
        void addTransformationRotateXTime( const GOAP::SourceInterfacePtr & _source, const TransformablePtr & _transformation, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, float _to, float _time, ETransformationRotateMode _mode, const DocumentInterfacePtr & _doc )
        {
            Cook::addTask<TaskTransformationRotateXTime>( _source, _transformation, _affectorable, _easing, _to, _time, _mode, _doc );
        }
        //////////////////////////////////////////////////////////////////////////
        void addTransformationRotateYTime( const GOAP::SourceInterfacePtr & _source, const TransformablePtr & _transformation, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, float _to, float _time, ETransformationRotateMode _mode, const DocumentInterfacePtr & _doc )
        {
            Cook::addTask<TaskTransformationRotateYTime>( _source, _transformation, _affectorable, _easing, _to, _time, _mode, _doc );
        }
        //////////////////////////////////////////////////////////////////////////
        void addTransformationRotateZTime( const GOAP::SourceInterfacePtr & _source, const TransformablePtr & _transformation, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, float _to, float _time, ETransformationRotateMode _mode, const DocumentInterfacePtr & _doc )
        {
            Cook::addTask<TaskTransformationRotateZTime>( _source, _transformation, _affectorable, _easing, _to, _time, _mode, _doc );
        }
        //////////////////////////////////////////////////////////////////////////
        void addTransformationScaleTime( const GOAP::SourceInterfacePtr & _source, const TransformablePtr & _transformation, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, const mt::vec3f & _to, float _time, uint32_t _flags, const DocumentInterfacePtr & _doc )
        {
            Cook::addTask<TaskTransformationScaleTime>( _source, _transformation, _affectorable, _easing, _to, _time, _flags, _doc );
        }
        //////////////////////////////////////////////////////////////////////////
        void addTransformationTranslate( const GOAP::SourceInterfacePtr & _source, const TransformablePtr & _transformation, const AffectorablePtr & _affectorable, const mt::vec3f & _to, float _speed, const DocumentInterfacePtr & _doc )
        {
            Cook::addTask<TaskTransformationTranslate>( _source, _transformation, _affectorable, _to, _speed, _doc );
        }
        //////////////////////////////////////////////////////////////////////////
        void addTransformationTranslateTime( const GOAP::SourceInterfacePtr & _source, const TransformablePtr & _transformation, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, const mt::vec3f & _to, float _time, const DocumentInterfacePtr & _doc )
        {
            Cook::addTask<TaskTransformationTranslateTime>( _source, _transformation, _affectorable, _easing, _to, _time, _doc );
        }
        //////////////////////////////////////////////////////////////////////////
        void addTransformationTranslateTimeWithSkip( const GOAP::SourceInterfacePtr & _source, const TransformablePtr & _transformation, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, const mt::vec3f & _to, float _time, const DocumentInterfacePtr & _doc )
        {
            Cook::addTask<TaskTransformationTranslateTimeWithSkip>( _source, _transformation, _affectorable, _easing, _to, _time, _doc );
        }
        //////////////////////////////////////////////////////////////////////////
        void addColorableAlphaTime( const GOAP::SourceInterfacePtr & _source, const ColorablePtr & _colorable, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, float _to, float _time, const DocumentInterfacePtr & _doc )
        {
            Cook::addTask<TaskColorableAlphaTime>( _source, _colorable, _affectorable, _easing, _to, _time, _doc );
        }
        //////////////////////////////////////////////////////////////////////////
        void addRenderableAlphaTime( const GOAP::SourceInterfacePtr & _source, const RenderablePtr & _renderable, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, const float _to, float _time, const DocumentInterfacePtr & _doc )
        {
            const RenderInterface * render = _renderable->getRender();

            Cook::addTask<TaskColorableAlphaTime>( _source, ColorablePtr::from( render ), _affectorable, _easing, _to, _time, _doc );
        }
        //////////////////////////////////////////////////////////////////////////
        void addPrint( const GOAP::SourceInterfacePtr & _source, const Char * _format, ... )
        {
            MENGINE_UNUSED( _source );
            MENGINE_UNUSED( _format );

#if defined(MENGINE_DEBUG)
            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _format );

            Cook::addTask<TaskPrint>( _source, _format, args );

            MENGINE_VA_LIST_END( args );
#endif
        }
        //////////////////////////////////////////////////////////////////////////
        void addReleasePrint( const GOAP::SourceInterfacePtr & _source, const Char * _format, ... )
        {
            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _format );

            Cook::addTask<TaskReleasePrint>( _source, _format, args );

            MENGINE_VA_LIST_END( args );
        }
        //////////////////////////////////////////////////////////////////////////
        void addDelay( const GOAP::SourceInterfacePtr & _source, const SchedulerInterfacePtr & _scheduler, float _time, const DocumentInterfacePtr & _doc )
        {
            Cook::addTask<TaskDelay>( _source, _scheduler, _time, _doc );
        }
        //////////////////////////////////////////////////////////////////////////
        void addGlobalDelay( const GOAP::SourceInterfacePtr & _source, float _time, const DocumentInterfacePtr & _doc )
        {
            Cook::addTask<TaskGlobalDelay>( _source, _time, _doc );
        }
        //////////////////////////////////////////////////////////////////////////
        void addLocalDelay( const GOAP::SourceInterfacePtr & _source, float _time, const DocumentInterfacePtr & _doc )
        {
            Cook::addTask<TaskLocalDelay>( _source, _time, _doc );
        }
        //////////////////////////////////////////////////////////////////////////
        void addGlobalKeyPress( const GOAP::SourceInterfacePtr & _source, EKeyCode _code, bool _isDown, const LambdaInputKeyEvent & _filter, const DocumentInterfacePtr & _doc )
        {
            Cook::addTask<TaskGlobalKeyPress>( _source, _code, _isDown, _filter, _doc );
        }
        //////////////////////////////////////////////////////////////////////////    
        void addGlobalMouseButton( const GOAP::SourceInterfacePtr & _source, EMouseButtonCode _button, bool _isDown, const LambdaInputMouseButtonEvent & _filter, const DocumentInterfacePtr & _doc )
        {
            Cook::addTask<TaskGlobalMouseButton>( _source, _button, _isDown, _filter, _doc );
        }
        //////////////////////////////////////////////////////////////////////////
        void addGlobalMouseMove( const GOAP::SourceInterfacePtr & _source, const LambdaInputMouseMoveEvent & _filter, const DocumentInterfacePtr & _doc )
        {
            Cook::addTask<TaskGlobalMouseMove>( _source, _filter, _doc );
        }
        //////////////////////////////////////////////////////////////////////////
        void addGlobalMouseWheel( const GOAP::SourceInterfacePtr & _source, const LambdaInputMouseWheelEvent & _filter, const DocumentInterfacePtr & _doc )
        {
            Cook::addTask<TaskGlobalMouseWheel>( _source, _filter, _doc );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}