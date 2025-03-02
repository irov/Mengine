#include "PickerService.h"

#include "Interface/InputServiceInterface.h"
#include "Interface/ArrowServiceInterface.h"

#include "Kernel/EnumeratorHelper.h"
#include "Kernel/Scene.h"
#include "Kernel/VectorAuxScope.h"
#include "Kernel/Logger.h"
#include "Kernel/VectorAuxScope.h"
#include "Kernel/IntrusivePtrView.h"
#include "Kernel/Assertion.h"
#include "Kernel/RenderContextHelper.h"
#include "Kernel/MixinDebug.h"
#include "Kernel/ProfilerHelper.h"
#include "Kernel/ResolutionHelper.h"

#include "Config/StdAlgorithm.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( PickerService, Mengine::PickerService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        class PickerVisitor
        {
        public:
            PickerVisitor( VectorPickerStates * _states, bool _exclusive )
                : m_states( _states )
                , m_exclusive( _exclusive )
            {
            }

            ~PickerVisitor()
            {
            }

        protected:
            void operator = ( const PickerVisitor & ) = delete;

        public:
            void visit( PickerInterface * _picker, const RenderContext & _context )
            {
                if( m_exclusive == true && _picker->isPickerExclusive() == false )
                {
                    return;
                }

                PickerStateDesc desc;

                desc.picker = _picker;

                desc.context.target = nullptr;
                desc.context.transformation = nullptr;

                const RenderViewportInterfacePtr & pickerViewport = _picker->getPickerViewport();

                if( pickerViewport != nullptr )
                {
                    desc.context.viewport = pickerViewport.get();
                }
                else
                {
                    desc.context.viewport = _context.viewport;
                }

                const RenderCameraInterfacePtr & pickerCamera = _picker->getPickerCamera();

                if( pickerCamera != nullptr )
                {
                    desc.context.camera = pickerCamera.get();
                }
                else
                {
                    desc.context.camera = _context.camera;
                }

                const RenderTransformationInterfacePtr & pickerTransformation = _picker->getPickerTransformation();

                if( pickerTransformation != nullptr )
                {
                    desc.context.transformation = pickerTransformation.get();
                }
                else
                {
                    desc.context.transformation = _context.transformation;
                }

                const RenderScissorInterfacePtr & pickerScissor = _picker->getPickerScissor();

                if( pickerScissor != nullptr )
                {
                    desc.context.scissor = pickerScissor.get();
                }
                else
                {
                    desc.context.scissor = _context.scissor;
                }

                const RenderTargetInterfacePtr & pickerTarget = _picker->getPickerTarget();

                if( pickerTarget != nullptr )
                {
                    desc.context.target = pickerTarget.get();
                }
                else
                {
                    desc.context.target = _context.target;
                }

                desc.context.zGroup = _context.zGroup;
                desc.context.zIndex = _context.zIndex;

                if( _picker->isPickerDummy() == false )
                {
                    m_states->emplace_back( desc );
                }

                _picker->foreachPickerChildrenEnabled( [this, desc]( PickerInterface * _picker )
                {
                    this->visit( _picker, desc.context );
                } );
            }

        protected:
            VectorPickerStates * m_states;

            bool m_exclusive;
        };
    }
    //////////////////////////////////////////////////////////////////////////
    PickerService::PickerService()
        : m_block( false )
        , m_handleValue( true )
        , m_invalidateTraps( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PickerService::~PickerService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickerService::_initializeService()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PickerService::_finalizeService()
    {
        m_scene = nullptr;

        m_viewport = nullptr;
        m_camera = nullptr;
        m_transformation = nullptr;
        m_scissor = nullptr;
        m_target = nullptr;

        MENGINE_ASSERTION_FATAL( m_states.empty() == true, "not all pickers removed" );
    }
    //////////////////////////////////////////////////////////////////////////
    void PickerService::setBlock( bool _value )
    {
        if( m_block == _value )
        {
            return;
        }

        m_block = _value;

        this->invalidateTraps();
    }
    //////////////////////////////////////////////////////////////////////////
    void PickerService::setHandleValue( bool _value )
    {
        if( m_handleValue == _value )
        {
            return;
        }

        m_handleValue = _value;

        this->invalidateTraps();
    }
    //////////////////////////////////////////////////////////////////////////
    void PickerService::setScene( const ScenePtr & _scene )
    {
        if( m_scene == _scene )
        {
            return;
        }

        m_scene = _scene;

        this->invalidateTraps();
    }
    //////////////////////////////////////////////////////////////////////////
    void PickerService::setRenderViewport( const RenderViewportInterfacePtr & _viewport )
    {
        if( m_viewport == _viewport )
        {
            return;
        }

        m_viewport = _viewport;

        this->invalidateTraps();
    }
    //////////////////////////////////////////////////////////////////////////
    void PickerService::setRenderCamera( const RenderCameraInterfacePtr & _camera )
    {
        if( m_camera == _camera )
        {
            return;
        }

        m_camera = _camera;

        this->invalidateTraps();
    }
    //////////////////////////////////////////////////////////////////////////
    void PickerService::setRenderTransformation( const RenderTransformationInterfacePtr & _transformation )
    {
        if( m_transformation == _transformation )
        {
            return;
        }

        m_transformation = _transformation;

        this->invalidateTraps();
    }
    //////////////////////////////////////////////////////////////////////////
    void PickerService::setRenderScissor( const RenderScissorInterfacePtr & _scissor )
    {
        if( m_scissor == _scissor )
        {
            return;
        }

        m_scissor = _scissor;

        this->invalidateTraps();
    }
    //////////////////////////////////////////////////////////////////////////
    void PickerService::setRenderTarget( const RenderTargetInterfacePtr & _target )
    {
        if( m_target == _target )
        {
            return;
        }

        m_target = _target;

        this->invalidateTraps();
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickerService::pickTraps( const mt::vec2f & _point, ETouchCode _touchId, float _pressure, const InputSpecialData & _special, VectorPickers * const _pickers ) const
    {
        VectorPickerStates statesAux;
        if( this->pickStates_( _point.x, _point.y, _touchId, _pressure, _special, &statesAux ) == false )
        {
            return false;
        }

        for( VectorPickerStates::reverse_iterator
            it = statesAux.rbegin(),
            it_end = statesAux.rend();
            it != it_end;
            ++it )
        {
            PickerStateDesc & desc = *it;

            PickerInterface * picker = desc.picker;

            if( picker->isPickerPicked() == false )
            {
                continue;
            }

            _pickers->emplace_back( picker );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickerService::getTraps( const mt::vec2f & _point, VectorPickers * const _pickers ) const
    {
        VectorPickerStates statesAux;
        if( this->getStates_( _point.x, _point.y, &statesAux ) == false )
        {
            return false;
        }

        for( const PickerStateDesc & desc : statesAux )
        {
            const PickerInterface * picker = desc.picker;

            _pickers->emplace_back( picker );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PickerService::update()
    {
        MENGINE_PROFILER_CATEGORY();

        if( m_invalidateTraps == true )
        {
            this->updateTraps_();

            m_invalidateTraps = false;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void PickerService::updateTraps_()
    {
        ETouchCode touchId = TC_TOUCH0;

        const mt::vec2f & position = INPUT_SERVICE()
            ->getCursorPosition( touchId );

        float pressure = INPUT_SERVICE()
            ->getCursorPressure( touchId );

        InputSpecialData special;
        INPUT_SERVICE()
            ->getSpecial( &special );

        VectorPickerStates statesAux;
        this->pickStates_( position.x, position.y, touchId, pressure, special, &statesAux );
    }
    //////////////////////////////////////////////////////////////////////////
    void PickerService::invalidateTraps()
    {
        m_invalidateTraps = true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickerService::handleKeyEvent( const InputKeyEvent & _event )
    {
        MENGINE_VECTOR_AUX( m_states );

        if( this->pickStates_( _event.x, _event.y, TC_TOUCH0, 0.f, _event.special, &m_states ) == false )
        {
            return false;
        }

        MENGINE_PROFILER_CATEGORY();

        for( VectorPickerStates::reverse_iterator
            it = m_states.rbegin(),
            it_end = m_states.rend();
            it != it_end;
            ++it )
        {
            PickerStateDesc & desc = *it;

            PickerInterface * picker = desc.picker;

            if( picker->isPickerEnable() == false )
            {
                continue;
            }

            PickerInputHandlerInterface * inputHandler = picker->getPickerInputHandler();

            mt::vec2f wp;
            ARROW_SERVICE()
                ->calcPointClick( &desc.context, mt::vec2f( _event.x, _event.y ), &wp );

            InputKeyEvent ne = _event;
            ne.x = wp.x;
            ne.y = wp.y;

            LOGGER_INFO( "picker", "handle type '%s' name '%s' UID [%u] pos [%.4f;%.4f] [key]"
                , MENGINE_MIXIN_DEBUG_TYPE( inputHandler )
                , MENGINE_MIXIN_DEBUG_NAME( inputHandler )
                , MENGINE_MIXIN_DEBUG_UID( inputHandler )
                , ne.x
                , ne.y
            );

            if( inputHandler->handleKeyEvent( &desc.context, ne ) == false )
            {
                continue;
            }

            return m_handleValue;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickerService::handleTextEvent( const InputTextEvent & _event )
    {
        MENGINE_VECTOR_AUX( m_states );

        if( this->pickStates_( _event.x, _event.y, TC_TOUCH0, 0.f, _event.special, &m_states ) == false )
        {
            return false;
        }

        MENGINE_PROFILER_CATEGORY();

        for( VectorPickerStates::reverse_iterator
            it = m_states.rbegin(),
            it_end = m_states.rend();
            it != it_end;
            ++it )
        {
            PickerStateDesc & desc = *it;

            PickerInterface * picker = desc.picker;

            if( picker->isPickerEnable() == false )
            {
                continue;
            }

            PickerInputHandlerInterface * inputHandler = picker->getPickerInputHandler();

            mt::vec2f wp;
            ARROW_SERVICE()
                ->calcPointClick( &desc.context, mt::vec2f( _event.x, _event.y ), &wp );

            InputTextEvent ne = _event;
            ne.x = wp.x;
            ne.y = wp.y;

            LOGGER_INFO( "picker", "handle type '%s' name '%s' UID [%u] pos [%.4f;%.4f] [text]"
                , MENGINE_MIXIN_DEBUG_TYPE( inputHandler )
                , MENGINE_MIXIN_DEBUG_NAME( inputHandler )
                , MENGINE_MIXIN_DEBUG_UID( inputHandler )
                , ne.x
                , ne.y
            );

            if( inputHandler->handleTextEvent( &desc.context, ne ) == false )
            {
                continue;
            }

            return m_handleValue;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickerService::handleAccelerometerEvent( const InputAccelerometerEvent & _event )
    {
        MENGINE_UNUSED( _event );

        //Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickerService::handleMouseButtonEvent( const InputMouseButtonEvent & _event )
    {
        MENGINE_VECTOR_AUX( m_states );

        if( this->pickStates_( _event.x, _event.y, _event.touchId, _event.pressure, _event.special, &m_states ) == false )
        {
            return false;
        }

        MENGINE_PROFILER_CATEGORY();

        for( VectorPickerStates::reverse_iterator
            it = m_states.rbegin(),
            it_end = m_states.rend();
            it != it_end;
            ++it )
        {
            const PickerStateDesc & desc = *it;

            PickerInterface * picker = desc.picker;

            if( picker->isPickerEnable() == false )
            {
                continue;
            }

            if( picker->isPickerPicked() == false )
            {
                continue;
            }

            mt::vec2f wp;
            ARROW_SERVICE()
                ->calcPointClick( &desc.context, mt::vec2f( _event.x, _event.y ), &wp );

            PickerInputHandlerInterface * inputHandler = picker->getPickerInputHandler();

            InputMouseButtonEvent ne = _event;
            ne.x = wp.x;
            ne.y = wp.y;
            ne.isPressed = picker->isPickerPressed();

            LOGGER_INFO( "picker", "handle type '%s' name '%s' UID [%u] pos [%.4f;%.4f] [mouse button]"
                , MENGINE_MIXIN_DEBUG_TYPE( inputHandler )
                , MENGINE_MIXIN_DEBUG_NAME( inputHandler )
                , MENGINE_MIXIN_DEBUG_UID( inputHandler )
                , ne.x
                , ne.y
            );

            if( inputHandler->handleMouseButtonEvent( &desc.context, ne ) == false )
            {
                continue;
            }

            return m_handleValue;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickerService::handleMouseButtonEventBegin( const InputMouseButtonEvent & _event )
    {
        MENGINE_VECTOR_AUX( m_states );

        if( this->pickStates_( _event.x, _event.y, _event.touchId, _event.pressure, _event.special, &m_states ) == false )
        {
            return false;
        }

        MENGINE_PROFILER_CATEGORY();

        for( VectorPickerStates::reverse_iterator
            it = m_states.rbegin(),
            it_end = m_states.rend();
            it != it_end;
            ++it )
        {
            const PickerStateDesc & desc = *it;

            PickerInterface * picker = desc.picker;

            if( picker->isPickerEnable() == false )
            {
                continue;
            }

            if( picker->isPickerPicked() == false )
            {
                continue;
            }

            if( _event.isDown == true )
            {
                picker->setPickerPressed( true );
            }

            mt::vec2f wp;
            ARROW_SERVICE()
                ->calcPointClick( &desc.context, mt::vec2f( _event.x, _event.y ), &wp );

            PickerInputHandlerInterface * inputHandler = picker->getPickerInputHandler();

            InputMouseButtonEvent ne = _event;
            ne.x = wp.x;
            ne.y = wp.y;
            ne.isPressed = picker->isPickerPressed();

            LOGGER_INFO( "picker", "handle type '%s' name '%s' UID [%u] pos [%.4f;%.4f] [mouse button begin]"
                , MENGINE_MIXIN_DEBUG_TYPE( inputHandler )
                , MENGINE_MIXIN_DEBUG_NAME( inputHandler )
                , MENGINE_MIXIN_DEBUG_UID( inputHandler )
                , ne.x
                , ne.y
            );

            if( inputHandler->handleMouseButtonEventBegin( &desc.context, ne ) == false )
            {
                continue;
            }

            return m_handleValue;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickerService::handleMouseButtonEventEnd( const InputMouseButtonEvent & _event )
    {
        MENGINE_VECTOR_AUX( m_states );

        if( this->pickStates_( _event.x, _event.y, _event.touchId, _event.pressure, _event.special, &m_states ) == false )
        {
            return false;
        }

        MENGINE_PROFILER_CATEGORY();

        for( VectorPickerStates::reverse_iterator
            it = m_states.rbegin(),
            it_end = m_states.rend();
            it != it_end;
            ++it )
        {
            const PickerStateDesc & desc = *it;

            PickerInterface * picker = desc.picker;

            if( picker->isPickerEnable() == false )
            {
                continue;
            }

            if( picker->isPickerPicked() == false )
            {
                continue;
            }

            if( _event.isDown == false )
            {
                if( picker->isPickerPressed() == false )
                {
                    //continue;
                }
                else
                {
                    picker->setPickerPressed( false );
                }
            }

            mt::vec2f wp;
            ARROW_SERVICE()
                ->calcPointClick( &desc.context, mt::vec2f( _event.x, _event.y ), &wp );

            PickerInputHandlerInterface * inputHandler = picker->getPickerInputHandler();

            InputMouseButtonEvent ne = _event;
            ne.x = wp.x;
            ne.y = wp.y;
            ne.isPressed = picker->isPickerPressed();

            LOGGER_INFO( "picker", "handle type '%s' name '%s' UID [%u] pos [%.4f;%.4f] [mouse button end]"
                , MENGINE_MIXIN_DEBUG_TYPE( inputHandler )
                , MENGINE_MIXIN_DEBUG_NAME( inputHandler )
                , MENGINE_MIXIN_DEBUG_UID( inputHandler )
                , ne.x
                , ne.y
            );

            if( inputHandler->handleMouseButtonEventEnd( &desc.context, ne ) == false )
            {
                continue;
            }

            return m_handleValue;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickerService::handleMouseMove( const InputMouseMoveEvent & _event )
    {
        MENGINE_VECTOR_AUX( m_states );

        if( this->pickStates_( _event.x, _event.y, _event.touchId, _event.pressure, _event.special, &m_states ) == false )
        {
            return false;
        }

        MENGINE_PROFILER_CATEGORY();

        for( VectorPickerStates::reverse_iterator
            it = m_states.rbegin(),
            it_end = m_states.rend();
            it != it_end;
            ++it )
        {
            const PickerStateDesc & desc = *it;

            PickerInterface * picker = desc.picker;

            if( picker->isPickerEnable() == false )
            {
                continue;
            }

            if( picker->isPickerPicked() == false )
            {
                continue;
            }

            mt::vec2f wp;
            ARROW_SERVICE()
                ->calcPointClick( &desc.context, mt::vec2f( _event.x, _event.y ), &wp );

            mt::vec2f dp;
            ARROW_SERVICE()
                ->calcPointDelta( &desc.context, mt::vec2f( _event.dx, _event.dy ), &dp );

            PickerInputHandlerInterface * inputHandler = picker->getPickerInputHandler();

            InputMouseMoveEvent ne = _event;
            ne.x = wp.x;
            ne.y = wp.y;
            ne.dx = dp.x;
            ne.dy = dp.y;

            LOGGER_INFO( "picker", "handle type '%s' name '%s' UID [%u] pos [%.4f;%.4f] [mouse move]"
                , MENGINE_MIXIN_DEBUG_TYPE( inputHandler )
                , MENGINE_MIXIN_DEBUG_NAME( inputHandler )
                , MENGINE_MIXIN_DEBUG_UID( inputHandler )
                , ne.x
                , ne.y
            );

            if( inputHandler->handleMouseMove( &desc.context, ne ) == false )
            {
                continue;
            }

            return m_handleValue;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickerService::handleMouseWheel( const InputMouseWheelEvent & _event )
    {
        MENGINE_VECTOR_AUX( m_states );

        if( this->pickStates_( _event.x, _event.y, TC_TOUCH0, 0.f, _event.special, &m_states ) == false )
        {
            return false;
        }

        MENGINE_PROFILER_CATEGORY();

        for( VectorPickerStates::reverse_iterator
            it = m_states.rbegin(),
            it_end = m_states.rend();
            it != it_end;
            ++it )
        {
            const PickerStateDesc & desc = *it;

            PickerInterface * picker = desc.picker;

            if( picker->isPickerEnable() == false )
            {
                continue;
            }

            if( picker->isPickerPicked() == false )
            {
                continue;
            }

            mt::vec2f wp;
            ARROW_SERVICE()
                ->calcPointClick( &desc.context, mt::vec2f( _event.x, _event.y ), &wp );

            PickerInputHandlerInterface * inputHandler = picker->getPickerInputHandler();

            InputMouseWheelEvent ne = _event;
            ne.x = wp.x;
            ne.y = wp.y;

            LOGGER_INFO( "picker", "handle type '%s' name '%s' UID [%u] pos [%.4f;%.4f] [mouse wheel]"
                , MENGINE_MIXIN_DEBUG_TYPE( inputHandler )
                , MENGINE_MIXIN_DEBUG_NAME( inputHandler )
                , MENGINE_MIXIN_DEBUG_UID( inputHandler )
                , ne.x
                , ne.y
            );

            if( inputHandler->handleMouseWheel( &desc.context, ne ) == false )
            {
                continue;
            }

            return m_handleValue;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickerService::handleMouseEnter( const InputMouseEnterEvent & _event )
    {
        MENGINE_VECTOR_AUX( m_states );

        this->pickStates_( _event.x, _event.y, _event.touchId, _event.pressure, _event.special, &m_states );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void PickerService::handleMouseLeave( const InputMouseLeaveEvent & _event )
    {
        if( m_scene == nullptr )
        {
            return;
        }

        MENGINE_VECTOR_AUX( m_states );

        this->fillStates_( &m_states );

        for( VectorPickerStates::reverse_iterator
            it = m_states.rbegin(),
            it_end = m_states.rend();
            it != it_end;
            ++it )
        {
            PickerStateDesc & desc = *it;

            PickerInterface * picker = desc.picker;

            if( picker->isPickerEnable() == false )
            {
                continue;
            }

            if( picker->isPickerPicked() == false )
            {
                continue;
            }

            picker->setPickerPicked( false );

            PickerInputHandlerInterface * inputHandler = picker->getPickerInputHandler();

            mt::vec2f wp;
            ARROW_SERVICE()
                ->calcPointClick( &desc.context, mt::vec2f( _event.x, _event.y ), &wp );

            InputMouseLeaveEvent ne = _event;
            ne.x = wp.x;
            ne.y = wp.y;

            LOGGER_INFO( "picker", "handle type '%s' name '%s' UID [%u] pos [%.4f;%.4f] [mouse leave]"
                , MENGINE_MIXIN_DEBUG_TYPE( inputHandler )
                , MENGINE_MIXIN_DEBUG_NAME( inputHandler )
                , MENGINE_MIXIN_DEBUG_UID( inputHandler )
                , ne.x
                , ne.y
            );

            inputHandler->handleMouseLeave( &desc.context, _event );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void PickerService::fillStates_( VectorPickerStates * const _states ) const
    {
        PickerInterface * picker = m_scene->getPicker();

        Detail::PickerVisitor visitor( _states, false );

        RenderContext context;
        Helper::clearRenderContext( &context );

        context.viewport = m_viewport.get();
        context.camera = m_camera.get();
        context.scissor = m_scissor.get();

        visitor.visit( picker, context );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickerService::pickStates_( float _x, float _y, ETouchCode _touchId, float _pressure, const InputSpecialData & _special, VectorPickerStates * const _states ) const
    {
        MENGINE_ASSERTION_FATAL( _states->empty() == true, "states not empty" );

        MENGINE_PROFILER_CATEGORY();

        if( m_scene == nullptr )
        {
            return false;
        }

        if( m_camera == nullptr )
        {
            return false;
        }

        this->fillStates_( _states );

        mt::vec2f adapt_screen_position;
        Helper::adaptScreenPosition( mt::vec2f( _x, _y ), &adapt_screen_position );

        bool handle = false;

        for( VectorPickerStates::reverse_iterator
            it = _states->rbegin(),
            it_end = _states->rend();
            it != it_end;
            ++it )
        {
            const PickerStateDesc & desc = *it;

            PickerInterface * picker = desc.picker;

            if( picker->isPickerEnable() == false )
            {
                continue;
            }

            if( handle == false || m_handleValue == false )
            {
                bool picked = picker->pick( adapt_screen_position, &desc.context );

                if( m_block == false && picked == true )
                {
                    if( picker->isPickerPicked() == false )
                    {
                        picker->setPickerPicked( true );

                        PickerInputHandlerInterface * inputHandler = picker->getPickerInputHandler();

                        mt::vec2f wp;
                        ARROW_SERVICE()
                            ->calcPointClick( &desc.context, mt::vec2f( _x, _y ), &wp );

                        InputMouseEnterEvent ne;
                        ne.special = _special;
                        ne.touchId = _touchId;
                        ne.x = wp.x;
                        ne.y = wp.y;
                        ne.pressure = _pressure;

                        LOGGER_INFO( "picker", "handle type '%s' name '%s' UID [%u] pos [%.4f;%.4f] [mouse enter]"
                            , MENGINE_MIXIN_DEBUG_TYPE( inputHandler )
                            , MENGINE_MIXIN_DEBUG_NAME( inputHandler )
                            , MENGINE_MIXIN_DEBUG_UID( inputHandler )
                            , ne.x
                            , ne.y
                        );

                        handle = inputHandler->handleMouseEnter( &desc.context, ne );

                        picker->setPickerHandle( handle );
                    }
                    else
                    {
                        bool pickerHandle = picker->isPickerHandle();

                        handle = pickerHandle;
                    }
                }
                else
                {
                    if( picker->isPickerPicked() == true )
                    {
                        picker->setPickerPicked( false );

                        PickerInputHandlerInterface * inputHandler = picker->getPickerInputHandler();

                        mt::vec2f wp;
                        ARROW_SERVICE()
                            ->calcPointClick( &desc.context, mt::vec2f( _x, _y ), &wp );

                        InputMouseLeaveEvent ne;
                        ne.special = _special;
                        ne.touchId = _touchId;
                        ne.x = wp.x;
                        ne.y = wp.y;
                        ne.pressure = _pressure;

                        LOGGER_INFO( "picker", "handle type '%s' name '%s' UID [%u] pos [%.4f;%.4f] [mouse leave]"
                            , MENGINE_MIXIN_DEBUG_TYPE( inputHandler )
                            , MENGINE_MIXIN_DEBUG_NAME( inputHandler )
                            , MENGINE_MIXIN_DEBUG_UID( inputHandler )
                            , ne.x
                            , ne.y
                        );

                        inputHandler->handleMouseLeave( &desc.context, ne );
                    }
                }
            }
            else
            {
                if( picker->isPickerPicked() == true )
                {
                    picker->setPickerPicked( false );

                    PickerInputHandlerInterface * inputHandler = picker->getPickerInputHandler();

                    mt::vec2f wp;
                    ARROW_SERVICE()
                        ->calcPointClick( &desc.context, mt::vec2f( _x, _y ), &wp );

                    InputMouseLeaveEvent ne;
                    ne.special = _special;
                    ne.touchId = _touchId;
                    ne.x = wp.x;
                    ne.y = wp.y;
                    ne.pressure = _pressure;

                    LOGGER_INFO( "picker", "handle type '%s' name '%s' UID [%u] pos [%.4f;%.4f] [mouse leave]"
                        , MENGINE_MIXIN_DEBUG_TYPE( inputHandler )
                        , MENGINE_MIXIN_DEBUG_NAME( inputHandler )
                        , MENGINE_MIXIN_DEBUG_UID( inputHandler )
                        , ne.x
                        , ne.y
                    );

                    inputHandler->handleMouseLeave( &desc.context, ne );
                }
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickerService::getStates_( float _x, float _y, VectorPickerStates * const _states ) const
    {
        MENGINE_ASSERTION_FATAL( _states->empty() == true, "states not empty" );

        if( m_scene == nullptr )
        {
            return false;
        }

        if( m_camera == nullptr )
        {
            return false;
        }

        if( m_block == true )
        {
            return true;
        }

        VectorPickerStates statesAux;
        this->fillStates_( &statesAux );

        mt::vec2f adapt_screen_position;
        Helper::adaptScreenPosition( mt::vec2f( _x, _y ), &adapt_screen_position );

        for( const PickerStateDesc & desc : statesAux )
        {
            PickerInterface * picker = desc.picker;

            if( picker->isPickerEnable() == false )
            {
                continue;
            }

            bool picked = picker->pick( adapt_screen_position, &desc.context );

            if( picked == false )
            {
                continue;
            }

            _states->push_back( desc );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
