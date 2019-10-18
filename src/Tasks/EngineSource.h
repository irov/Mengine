#pragma once

#include "Interface/InputHandlerInterface.h"
#include "Interface/SchedulerInterface.h"

#include "TaskNotify.h"
#include "TaskEventable.h"

#include "Kernel/Animatable.h"
#include "Kernel/Eventable.h"
#include "Kernel/Node.h"

#include "Config/Metaprogramming.h"

#include "GOAP/Source.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<class EngineSource> EngineSourcePtr;
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<EngineSourcePtr> VectorEngineSources;
    //////////////////////////////////////////////////////////////////////////
    template<size_t Count>
    using ArrayEngineSources = GOAP::Array<EngineSourcePtr, Count>;
    //////////////////////////////////////////////////////////////////////////
    class EngineSource
        : public GOAP::Source
    {
    public:
        EngineSource( const GOAP::SourceProviderInterfacePtr & _provider );
        ~EngineSource() override;

    public:
        void addAnimatablePause( const AnimatablePtr & _animatable );
        void addAnimatablePlay( const AnimatablePtr & _animatable );
        void addAnimatablePlayWait( const AnimatablePtr & _animatable, const EventablePtr & _eventable );
        void addAnimatableResume( const AnimatablePtr & _animatable );
        void addAnimatableRewind( const AnimatablePtr & _animatable );
        void addAnimatableStop( const AnimatablePtr & _animatable );

        void addNodeDestroy( const NodePtr & _node );
        void addNodeDisable( const NodePtr & _node );
        void addNodeEnable( const NodePtr & _node );

        typedef Lambda<bool( const InputMouseButtonEvent & )> LambdaPickerMouseButtonEvent;
        void addPickerableMouseButton( const PickerablePtr & _pickerable, EMouseCode _code, bool _isDown, bool _isPressed, const LambdaPickerMouseButtonEvent & _filter );

        typedef Lambda<bool( const InputMouseEnterEvent & )> LambdaPickerMouseEnterEvent;
        void addPickerableMouseEnter( const PickerablePtr & _pickerable, const LambdaPickerMouseEnterEvent & _filter );

        typedef Lambda<void( const InputMouseLeaveEvent & )> LambdaPickerMouseLeaveEvent;
        void addPickerableMouseLeave( const PickerablePtr & _pickerable, const LambdaPickerMouseLeaveEvent & _filter );

        void addTransformationRotateY( const TransformationPtr & _transformation, const AffectorablePtr & _affectorable, float _to, float _speed );
        void addTransformationRotateYTime( const TransformationPtr & _transformation, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, float _to, float _time );
        void addTransformationScaleTime( const TransformationPtr & _transformation, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, const mt::vec3f & _to, float _time );
        void addTransformationTranslate( const TransformationPtr & _transformation, const AffectorablePtr & _affectorable, const mt::vec3f & _to, float _speed );
        void addTransformationTranslateTime( const TransformationPtr & _transformation, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, const mt::vec3f & _to, float _time );

        void addPrint( const Char * _format, ... );

        void addDelay( const SchedulerInterfacePtr & _scheduler, float _time );
        void addGlobalDelay( float _time );
        void addLocalDelay( float _time );

        template<class ID, class ... Args>
        void addNotify( Args && ... _args )
        {
            this->addTask<TaskNotify>( std::forward<Args &&>( _args ) ... );
        }

        template<class M, class ... Args>
        void addEventable( Eventable * _eventable, uint32_t _event, M _method, Args && ... _args )
        {
            this->addTask<TaskEventable>( _eventable, _event, _method, std::forward<Args &&>( _args ) ... );
        }

        using GOAP::Source::addFunction;

        template<class P, class D, class M, class ... Args>
        void addFunction( const IntrusivePtr<P, D> & _self, M _method, Args && ... _args )
        {
            GOAP::FunctionProviderPtr provider = GOAP::Helper::makeFunctionProvider( [&, _self, _method, _args ...](){ P * p = _self.get(); (p->*_method)(_args ...); } );

            this->addFunctionProvider( provider );
        }

        using GOAP::Source::addFunctionContext;

        template<class P, class D, class M, class ... Args>
        void addFunctionContext( const IntrusivePtr<P, D> & _self, M _method, Args && ... _args )
        {
            GOAP::FunctionContextProviderPtr provider = GOAP::Helper::makeFunctionContextProvider( [&, _self, _method, _args ...]( bool _skip ){ P * p = _self.get(); (p->*_method)(_skip, _args ...); } );

            this->addFunctionContextProvider( provider );
        }

        using GOAP::Source::addCallback;

        template<class P, class D, class M, class ... Args>
        void addCallback( const IntrusivePtr<P, D> & _self, M _method, Args && ... _args )
        {
            GOAP::CallbackProviderPtr provider = GOAP::Helper::makeCallbackProvider( [&, _self, _method, _args ...]( const GOAP::CallbackObserverPtr & _callback, bool _skip ){ P * p = _self.get(); (p->*_method)(_callback, _skip, _args ...); } );

            this->addCallbackProvider( provider );
        }

        using GOAP::Source::addScope;

        template<class P, class D, class M, class ... Args>
        void addScope( const IntrusivePtr<P, D> & _self, M _method, Args && ... _args )
        {
            GOAP::ScopeProviderPtr provider = GOAP::Helper::makeScopeProvider( [&, _self, _method, _args ...]( const GOAP::SourcePtr & _source ){ P * p = _self.get(); (p->*_method)(_source, _args ...); } );

            this->addScopeProvider( provider );
        }

        template<class F>
        ArrayEngineSources<2> addIf( F _f )
        {
            GOAP::IfProviderPtr provider = GOAP::Helper::makeIfProvider( _f );

            GOAP::ArraySources<2> desc = this->addIfProvider( provider );

            return mpl::array_cast<EngineSourcePtr>(desc);
        }

        template<class P, class D, class M, class ... Args>
        ArrayEngineSources<2> addIf( const IntrusivePtr<P, D> & _self, M _method, Args && ... _args )
        {
            GOAP::IfProviderPtr provider = GOAP::Helper::makeIfProvider( [&, _self, _method, _args ...](){ P * p = _self.get(); return (p->*_method)(_args ...); } );

            GOAP::ArraySources<2> desc = this->addIfProvider( provider );

            return mpl::array_cast<EngineSourcePtr>(desc);
        }

        using GOAP::Source::addRace;

        template<size_t Count>
        ArrayEngineSources<Count> addRace()
        {
            GOAP::ArraySources<Count> sources;

            for( GOAP::SourcePtr & source : sources )
            {
                source = Helper::makeEngineSource();
            }

            GOAP::TranscriptorRaceArrayPtr<Count> transcriptor = GOAP::Helper::makeTranscriptorRaceArray( std::move( sources ) );

            m_provider->addTranscriptor( transcriptor );

            const GOAP::ArraySources<Count> & transcriptor_sources = transcriptor->getSources();

            return mpl::array_cast<EngineSourcePtr>(transcriptor_sources);
        }

        template<class C>
        GOAP::Zip<EngineSourcePtr, GOAP::VectorSources::const_iterator, typename C::iterator> addParallelZip( C & _c )
        {
            typename C::size_type zip_size = _c.size();
            const GOAP::VectorSources & parallel_sources = this->addParallel( (uint32_t)zip_size );

            GOAP::Zip<EngineSourcePtr, GOAP::VectorSources::const_iterator, typename C::iterator> z( parallel_sources.begin(), parallel_sources.end(), _c.begin(), _c.end() );

            return z;
        }

        template<class C>
        GOAP::Zip<EngineSourcePtr, GOAP::VectorSources::const_iterator, typename C::const_iterator> addParallelZip( const C & _c )
        {
            typename C::size_type zip_size = _c.size();
            const GOAP::VectorSources & parallel_sources = this->addParallel( (uint32_t)zip_size );

            return GOAP::Zip<EngineSourcePtr, GOAP::VectorSources::const_iterator, typename C::const_iterator>( parallel_sources.begin(), parallel_sources.end(), _c.begin(), _c.end() );
        }

        template<class C>
        GOAP::Zip<EngineSourcePtr, GOAP::VectorSources::const_iterator, typename C::iterator> addRaceZip( C & _c )
        {
            typename C::size_type zip_size = _c.size();
            const GOAP::VectorSources & parallel_sources = this->addRace( (uint32_t)zip_size );

            return GOAP::Zip<EngineSourcePtr, GOAP::VectorSources::const_iterator, typename C::iterator>( parallel_sources.begin(), parallel_sources.end(), _c.begin(), _c.end() );
        }

        template<class C>
        GOAP::Zip<EngineSourcePtr, GOAP::VectorSources::const_iterator, typename C::const_iterator> addRaceZip( const C & _c )
        {
            typename C::size_type zip_size = _c.size();
            const GOAP::VectorSources & parallel_sources = this->addRace( (uint32_t)zip_size );

            return GOAP::Zip<EngineSourcePtr, GOAP::VectorSources::const_iterator, typename C::const_iterator>( parallel_sources.begin(), parallel_sources.end(), _c.begin(), _c.end() );
        }

        typedef Lambda<bool( const InputKeyEvent & )> LambdaInputKeyEvent;
        void addGlobalKeyPress( EKeyCode _code, bool _isDown, const LambdaInputKeyEvent & _filter );

        typedef Lambda<bool( const InputMouseButtonEvent & )> LambdaInputMouseButtonEvent;
        void addGlobalMouseButton( EMouseCode _code, bool _isDown, const LambdaInputMouseButtonEvent & _filter );

        typedef Lambda<bool( const InputMouseMoveEvent & )> LambdaInputMouseMoveEvent;
        void addGlobalMouseMove( const LambdaInputMouseMoveEvent & _filter );

        typedef Lambda<bool( const InputMouseWheelEvent & )> LambdaInputMouseWheelEvent;
        void addGlobalMouseWheel( const LambdaInputMouseWheelEvent & _filter );
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<EngineSource> EngineSourcePtr;
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        EngineSourcePtr makeEngineSource();
    }
}