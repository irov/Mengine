#pragma once

#include "Interface/InputHandlerInterface.h"
#include "Interface/SchedulerInterface.h"
#include "Interface/RenderInterface.h"

#include "TaskEnum.h"
#include "TaskNotify.h"
#include "TaskEventable.h"
#include "TransformationRotateMode.h"

#include "Kernel/IntrusivePtr.h"
#include "Kernel/Animatable.h"
#include "Kernel/Eventable.h"
#include "Kernel/Colorable.h"
#include "Kernel/Node.h"

#include "Config/Metaprogramming.h"

#include "GOAP/Cook.h"

namespace Mengine
{
    namespace Cook
    {
        using namespace GOAP::Cook;

        void addAnimatablePause( const GOAP::SourceInterfacePtr & _source, const AnimatablePtr & _animatable );
        void addAnimatablePlay( const GOAP::SourceInterfacePtr & _source, const AnimatablePtr & _animatable );
        void addAnimatablePlayWait( const GOAP::SourceInterfacePtr & _source, const AnimatablePtr & _animatable, const EventablePtr & _eventable, const DocumentInterfacePtr & _doc );
        void addAnimatableResume( const GOAP::SourceInterfacePtr & _source, const AnimatablePtr & _animatable );
        void addAnimatableRewind( const GOAP::SourceInterfacePtr & _source, const AnimatablePtr & _animatable );
        void addAnimatableStop( const GOAP::SourceInterfacePtr & _source, const AnimatablePtr & _animatable );
        void addNodeDestroy( const GOAP::SourceInterfacePtr & _source, const NodePtr & _node );
        void addNodeDisable( const GOAP::SourceInterfacePtr & _source, const NodePtr & _node );
        void addNodeEnable( const GOAP::SourceInterfacePtr & _source, const NodePtr & _node );

        typedef Lambda<bool( const RenderContext *, const InputMouseButtonEvent &, bool * )> LambdaPickerMouseButtonEvent;
        void addPickerableMouseButton( const GOAP::SourceInterfacePtr & _source, const PickerablePtr & _pickerable, EMouseButtonCode _button, bool _isDown, bool _isPressed, const LambdaPickerMouseButtonEvent & _filter, const DocumentInterfacePtr & _doc );

        typedef Lambda<bool( const RenderContext *, const InputMouseEnterEvent &, bool * )> LambdaPickerMouseEnterEvent;
        void addPickerableMouseEnter( const GOAP::SourceInterfacePtr & _source, const PickerablePtr & _pickerable, const LambdaPickerMouseEnterEvent & _filter, const DocumentInterfacePtr & _doc );

        typedef Lambda<bool( const RenderContext *, const InputMouseLeaveEvent & )> LambdaPickerMouseLeaveEvent;
        void addPickerableMouseLeave( const GOAP::SourceInterfacePtr & _source, const PickerablePtr & _pickerable, const LambdaPickerMouseLeaveEvent & _filter, const DocumentInterfacePtr & _doc );

        void addTransformationRotateX( const GOAP::SourceInterfacePtr & _source, const TransformablePtr & _transformation, const AffectorablePtr & _affectorable, float _to, float _speed, const DocumentInterfacePtr & _doc );
        void addTransformationRotateY( const GOAP::SourceInterfacePtr & _source, const TransformablePtr & _transformation, const AffectorablePtr & _affectorable, float _to, float _speed, const DocumentInterfacePtr & _doc );
        void addTransformationRotateZ( const GOAP::SourceInterfacePtr & _source, const TransformablePtr & _transformation, const AffectorablePtr & _affectorable, float _to, float _speed, const DocumentInterfacePtr & _doc );
        void addTransformationRotateXTime( const GOAP::SourceInterfacePtr & _source, const TransformablePtr & _transformation, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, float _to, float _time, ETransformationRotateMode _mode, const DocumentInterfacePtr & _doc );
        void addTransformationRotateYTime( const GOAP::SourceInterfacePtr & _source, const TransformablePtr & _transformation, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, float _to, float _time, ETransformationRotateMode _mode, const DocumentInterfacePtr & _doc );
        void addTransformationRotateZTime( const GOAP::SourceInterfacePtr & _source, const TransformablePtr & _transformation, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, float _to, float _time, ETransformationRotateMode _mode, const DocumentInterfacePtr & _doc );
        void addTransformationScaleTime( const GOAP::SourceInterfacePtr & _source, const TransformablePtr & _transformation, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, const mt::vec3f & _to, float _time, uint32_t _flags, const DocumentInterfacePtr & _doc );
        void addTransformationTranslate( const GOAP::SourceInterfacePtr & _source, const TransformablePtr & _transformation, const AffectorablePtr & _affectorable, const mt::vec3f & _to, float _speed, const DocumentInterfacePtr & _doc );
        void addTransformationTranslateTime( const GOAP::SourceInterfacePtr & _source, const TransformablePtr & _transformation, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, const mt::vec3f & _to, float _time, const DocumentInterfacePtr & _doc );
        void addTransformationTranslateTimeWithSkip( const GOAP::SourceInterfacePtr & _source, const TransformablePtr & _transformation, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, const mt::vec3f & _to, float _time, const DocumentInterfacePtr & _doc );

        void addColorableAlphaTime( const GOAP::SourceInterfacePtr & _source, const ColorablePtr & _colorable, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, const float _to, float _time, const DocumentInterfacePtr & _doc );
        void addRenderableAlphaTime( const GOAP::SourceInterfacePtr & _source, const RenderablePtr & _renderable, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, const float _to, float _time, const DocumentInterfacePtr & _doc );

        void addPrint( const GOAP::SourceInterfacePtr & _source, const Char * _format, ... ) MENGINE_ATTRIBUTE_FORMAT_STRING( 2, 3 );
        void addReleasePrint( const GOAP::SourceInterfacePtr & _source, const Char * _format, ... ) MENGINE_ATTRIBUTE_FORMAT_STRING( 2, 3 );

        void addDelay( const GOAP::SourceInterfacePtr & _source, const SchedulerInterfacePtr & _scheduler, float _time, const DocumentInterfacePtr & _doc );
        void addGlobalDelay( const GOAP::SourceInterfacePtr & _source, float _time, const DocumentInterfacePtr & _doc );
        void addLocalDelay( const GOAP::SourceInterfacePtr & _source, float _time, const DocumentInterfacePtr & _doc );
        //////////////////////////////////////////////////////////////////////////
        template<class ID, class ... Args>
        void addNotify( const GOAP::SourceInterfacePtr & _source, Args && ... _args )
        {
            Cook::addTask<TaskNotify>( _source, std::forward<Args>( _args ) ... );
        }
        //////////////////////////////////////////////////////////////////////////
        template<class M, class ... Args>
        void addEventable( const GOAP::SourceInterfacePtr & _source, Eventable * _eventable, uint32_t _event, M _method, Args && ... _args )
        {
            Cook::addTask<TaskEventable>( _source, _eventable, _event, _method, std::forward<Args>( _args ) ... );
        }
        //////////////////////////////////////////////////////////////////////////
        typedef Lambda<bool( const InputKeyEvent & )> LambdaInputKeyFilter;
        typedef Lambda<void( const InputKeyEvent & )> LambdaInputKeyComplete;
        //////////////////////////////////////////////////////////////////////////
        void addGlobalKeyPress( const GOAP::SourceInterfacePtr & _source, EKeyCode _code, bool _isDown, const LambdaInputKeyFilter & _filter, const LambdaInputKeyComplete & _cb, const DocumentInterfacePtr & _doc );
        //////////////////////////////////////////////////////////////////////////
        template<class C, class M, class ... Args>
        void addGlobalKeyPress( const GOAP::SourceInterfacePtr & _source, EKeyCode _code, bool _isDown, const LambdaInputKeyFilter & _filter, C * _class, M _method, Args && ... _args, const DocumentInterfacePtr & _doc )
        {
            LambdaInputKeyComplete cb = [_class, _method, _args ...]( const InputKeyEvent & _event )
            {
                (_class->*_method)(_event, _args ...);
            };

            Cook::addGlobalKeyPress( _source, _code, _isDown, _filter, cb, _doc );
        }
        //////////////////////////////////////////////////////////////////////////
        typedef Lambda<bool( const InputMouseButtonEvent & )> LambdaInputMouseButtonFilter;
        typedef Lambda<void( const InputMouseButtonEvent & )> LambdaInputMouseButtonComplete;
        //////////////////////////////////////////////////////////////////////////
        void addGlobalMouseButton( const GOAP::SourceInterfacePtr & _source, EMouseButtonCode _button, bool _isDown, const LambdaInputMouseButtonFilter & _filter, const LambdaInputMouseButtonComplete & _cb, const DocumentInterfacePtr & _doc );
        //////////////////////////////////////////////////////////////////////////
        template<class C, class M, class ... Args>
        void addGlobalMouseButton( const GOAP::SourceInterfacePtr & _source, EMouseButtonCode _button, bool _isDown, const LambdaInputMouseButtonFilter & _filter, C * _class, M _method, Args && ... _args, const DocumentInterfacePtr & _doc )
        {
            LambdaInputMouseButtonComplete cb = [_class, _method, _args ...]( const InputMouseButtonEvent & _event )
            {
                (_class->*_method)(_event, _args ...);
            };

            Cook::addGlobalMouseButton( _source, _button, _isDown, _filter, cb, _doc );
        }
        //////////////////////////////////////////////////////////////////////////
        typedef Lambda<bool( const InputMouseMoveEvent & )> LambdaInputMouseMoveFilter;
        void addGlobalMouseMove( const GOAP::SourceInterfacePtr & _source, const LambdaInputMouseMoveFilter & _filter, const DocumentInterfacePtr & _doc );
        //////////////////////////////////////////////////////////////////////////
        typedef Lambda<bool( const InputMouseWheelEvent & )> LambdaInputMouseWheelFilter;
        void addGlobalMouseWheel( const GOAP::SourceInterfacePtr & _source, const LambdaInputMouseWheelFilter & _filter, const DocumentInterfacePtr & _doc );
        //////////////////////////////////////////////////////////////////////////
        using GOAP::Cook::addFunction;
        //////////////////////////////////////////////////////////////////////////
        template<class P, class D, class M, class ... Args>
        void addFunction( const GOAP::SourceInterfacePtr & _source, const IntrusivePtr<P, D> & _self, M _method, Args && ... _args )
        {
            GOAP::Allocator * allocator = _source->getAllocator();

            GOAP::FunctionProviderInterfacePtr provider = GOAP::Helper::makeFunctionProvider( allocator, [_self, _method, _args ...]()
            {
                P * p = _self.get();

                (p->*_method)(_args ...);
            } );

            Cook::addFunctionProvider( _source, provider );
        }
        //////////////////////////////////////////////////////////////////////////
        using GOAP::Cook::addFunctionContext;
        //////////////////////////////////////////////////////////////////////////
        template<class P, class D, class M, class ... Args>
        void addFunctionContext( const GOAP::SourceInterfacePtr & _source, const IntrusivePtr<P, D> & _self, M _method, Args && ... _args )
        {
            GOAP::Allocator * allocator = _source->getAllocator();

            GOAP::FunctionContextProviderInterfacePtr provider = GOAP::Helper::makeFunctionContextProvider( allocator, [_self, _method, _args ...]( bool _skip )
            {
                P * p = _self.get();

                (p->*_method)(_skip, _args ...);
            } );

            Cook::addFunctionContextProvider( _source, provider );
        }
        //////////////////////////////////////////////////////////////////////////
        using GOAP::Cook::addCallback;
        //////////////////////////////////////////////////////////////////////////
        template<class P, class D, class M, class ... Args>
        void addCallback( const GOAP::SourceInterfacePtr & _source, const IntrusivePtr<P, D> & _self, M _method, Args && ... _args )
        {
            GOAP::Allocator * allocator = _source->getAllocator();

            GOAP::CallbackProviderInterfacePtr provider = GOAP::Helper::makeCallbackProvider( allocator, [&, _self, _method, _args ...]( const GOAP::CallbackObserverInterfacePtr & _callback, bool _skip )
            {
                P * p = _self.get();

                (p->*_method)(_callback, _skip, _args ...);
            } );

            Cook::addCallbackProvider( _source, provider );
        }
        //////////////////////////////////////////////////////////////////////////
        using GOAP::Cook::addScope;
        //////////////////////////////////////////////////////////////////////////
        template<class P, class D, class M, class ... Args>
        void addScope( const GOAP::SourceInterfacePtr & _source, const IntrusivePtr<P, D> & _self, M _method, Args && ... _args )
        {
            GOAP::Allocator * allocator = _source->getAllocator();

            GOAP::ScopeProviderInterfacePtr provider = GOAP::Helper::makeScopeProvider( allocator, [_self, _method, _args ...]( const GOAP::SourceInterfacePtr & _source )
            {
                P * p = _self.get();

                (p->*_method)(_source, _args ...);
            } );

            Cook::addScopeProvider( _source, provider );
        }
        //////////////////////////////////////////////////////////////////////////
        using GOAP::Cook::addIf;
        //////////////////////////////////////////////////////////////////////////
        template<class P, class D, class M, class ... Args>
        GOAP::ArraySources<2> addIf( const GOAP::SourceInterfacePtr & _source, const IntrusivePtr<P, D> & _self, M _method, Args && ... _args )
        {
            GOAP::Allocator * allocator = _source->getAllocator();

            GOAP::IfProviderInterfacePtr provider = GOAP::Helper::makeIfProvider( allocator, [_self, _method, _args ...]()
            {
                P * p = _self.get();

                return (p->*_method)(_args ...);
            } );

            GOAP::ArraySources<2> desc = Cook::addIfProvider( _source, provider );

            return desc;
        }
        //////////////////////////////////////////////////////////////////////////
        using GOAP::Cook::addRepeat;
        //////////////////////////////////////////////////////////////////////////
        template<class P, class D, class M, class ... Args>
        GOAP::SourceInterfacePtr addRepeat( const GOAP::SourceInterfacePtr & _source, const IntrusivePtr<P, D> & _self, M _method, Args && ... _args )
        {
            GOAP::Allocator * allocator = _source->getAllocator();

            GOAP::WhileProviderInterfacePtr provider = GOAP::Helper::makeWhileProvider( allocator, [_self, _method, _args ...]( const GOAP::SourceInterfacePtr & _source )
            {
                P * p = _self.get();

                return (p->*_method)(_source, _args ...);
            } );

            GOAP::SourceInterfacePtr source_until = _source->makeSource();

            Cook::addTaskRepeat( _source, provider, source_until );

            return source_until;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}