#pragma once

#include "Interface/InputHandlerInterface.h"
#include "Interface/SchedulerInterface.h"

#include "TaskEnum.h"
#include "TaskNotify.h"
#include "TaskEventable.h"

#include "Kernel/IntrusivePtr.h"
#include "Kernel/Animatable.h"
#include "Kernel/Eventable.h"
#include "Kernel/Colorable.h"
#include "Kernel/Node.h"

#include "Config/Metaprogramming.h"
#include "Config/Export.h"

#include "GOAP/Cook.h"

namespace Mengine
{
    namespace Cook
    {
        using namespace GOAP::Cook;

        void addAnimatablePause( const GOAP::SourceInterfacePtr & _source, const AnimatablePtr & _animatable );
        void addAnimatablePlay( const GOAP::SourceInterfacePtr & _source, const AnimatablePtr & _animatable );
        void addAnimatablePlayWait( const GOAP::SourceInterfacePtr & _source, const AnimatablePtr & _animatable, const EventablePtr & _eventable );
        void addAnimatableResume( const GOAP::SourceInterfacePtr & _source, const AnimatablePtr & _animatable );
        void addAnimatableRewind( const GOAP::SourceInterfacePtr & _source, const AnimatablePtr & _animatable );
        void addAnimatableStop( const GOAP::SourceInterfacePtr & _source, const AnimatablePtr & _animatable );
        void addNodeDestroy( const GOAP::SourceInterfacePtr & _source, const NodePtr & _node );
        void addNodeDisable( const GOAP::SourceInterfacePtr & _source, const NodePtr & _node );
        void addNodeEnable( const GOAP::SourceInterfacePtr & _source, const NodePtr & _node );

        typedef Lambda<bool( const InputMouseButtonEvent &, bool * )> LambdaPickerMouseButtonEvent;
        void addPickerableMouseButton( const GOAP::SourceInterfacePtr & _source, const PickerablePtr & _pickerable, EMouseCode _code, bool _isDown, bool _isPressed, const LambdaPickerMouseButtonEvent & _filter );

        typedef Lambda<bool( const InputMouseEnterEvent &, bool * )> LambdaPickerMouseEnterEvent;
        void addPickerableMouseEnter( const GOAP::SourceInterfacePtr & _source, const PickerablePtr & _pickerable, const LambdaPickerMouseEnterEvent & _filter );

        typedef Lambda<bool( const InputMouseLeaveEvent & )> LambdaPickerMouseLeaveEvent;
        void addPickerableMouseLeave( const GOAP::SourceInterfacePtr & _source, const PickerablePtr & _pickerable, const LambdaPickerMouseLeaveEvent & _filter );

        void addTransformationRotateY( const GOAP::SourceInterfacePtr & _source, const TransformationPtr & _transformation, const AffectorablePtr & _affectorable, float _to, float _speed );
        void addTransformationRotateYTime( const GOAP::SourceInterfacePtr & _source, const TransformationPtr & _transformation, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, float _to, float _time );
        void addTransformationScaleTime( const GOAP::SourceInterfacePtr & _source, const TransformationPtr & _transformation, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, const mt::vec3f & _to, float _time, uint32_t _flags = ETASK_FLAG_NONE );
        void addTransformationTranslate( const GOAP::SourceInterfacePtr & _source, const TransformationPtr & _transformation, const AffectorablePtr & _affectorable, const mt::vec3f & _to, float _speed );
        void addTransformationTranslateTime( const GOAP::SourceInterfacePtr & _source, const TransformationPtr & _transformation, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, const mt::vec3f & _to, float _time );

        void addColorableAlphaTime( const GOAP::SourceInterfacePtr & _source, const ColorablePtr & _transformation, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, const float _to, float _time );

        void addPrint( const GOAP::SourceInterfacePtr & _source, MENGINE_CHECK_FORMAT_STRING( const Char * _format ), ... );

        void addDelay( const GOAP::SourceInterfacePtr & _source, const SchedulerInterfacePtr & _scheduler, float _time );
        void addGlobalDelay( const GOAP::SourceInterfacePtr & _source, float _time );
        void addLocalDelay( const GOAP::SourceInterfacePtr & _source, float _time );
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
        typedef Lambda<bool( const InputKeyEvent & )> LambdaInputKeyEvent;
        void addGlobalKeyPress( const GOAP::SourceInterfacePtr & _source, EKeyCode _code, bool _isDown, const LambdaInputKeyEvent & _filter );
        //////////////////////////////////////////////////////////////////////////
        typedef Lambda<bool( const InputMouseButtonEvent & )> LambdaInputMouseButtonEvent;
        void addGlobalMouseButton( const GOAP::SourceInterfacePtr & _source, EMouseCode _code, bool _isDown, const LambdaInputMouseButtonEvent & _filter );
        //////////////////////////////////////////////////////////////////////////
        typedef Lambda<bool( const InputMouseMoveEvent & )> LambdaInputMouseMoveEvent;
        void addGlobalMouseMove( const GOAP::SourceInterfacePtr & _source, const LambdaInputMouseMoveEvent & _filter );
        //////////////////////////////////////////////////////////////////////////
        typedef Lambda<bool( const InputMouseWheelEvent & )> LambdaInputMouseWheelEvent;
        void addGlobalMouseWheel( const GOAP::SourceInterfacePtr & _source, const LambdaInputMouseWheelEvent & _filter );
        //////////////////////////////////////////////////////////////////////////
        using GOAP::Cook::addFunction;
        //////////////////////////////////////////////////////////////////////////
        template<class P, class D, class M, class ... Args>
        void addFunction( const GOAP::SourceInterfacePtr & _source, const IntrusivePtr<P, D> & _self, M _method, Args && ... _args )
        {
            GOAP::Allocator * allocator = _source->getAllocator();

            GOAP::FunctionProviderInterfacePtr provider = GOAP::Helper::makeFunctionProvider( allocator, [&, _self, _method, _args ...]()
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

            GOAP::FunctionContextProviderInterfacePtr provider = GOAP::Helper::makeFunctionContextProvider( allocator, [&, _self, _method, _args ...]( bool _skip )
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

            GOAP::ScopeProviderInterfacePtr provider = GOAP::Helper::makeScopeProvider( allocator, [&, _self, _method, _args ...]( const GOAP::SourceInterfacePtr & _source )
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

            GOAP::IfProviderInterfacePtr provider = GOAP::Helper::makeIfProvider( allocator, [&, _self, _method, _args ...]()
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

            GOAP::WhileProviderInterfacePtr provider = GOAP::Helper::makeWhileProvider( allocator, [&, _self, _method, _args ...]( const GOAP::SourceInterfacePtr & _source )
            {
                P * p = _self.get();
                return (p->*_method)(_source, _args ...);
            } );

            GOAP::SourceInterfacePtr source_until = _source->makeSource();

            Cook::addTaskRepeat( _source, provider, source_until );

            return source_until;
        }
    }
}