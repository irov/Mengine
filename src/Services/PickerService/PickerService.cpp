#include "PickerService.h"

#include "Interface/InputServiceInterface.h"
#include "Interface/ArrowServiceInterface.h"

#include "Kernel/EnumeratorHelper.h"
#include "Kernel/Scene.h"
#include "Kernel/VectorAuxScope.h"
#include "Kernel/Logger.h"
#include "Kernel/IntrusivePtrView.h"
#include "Kernel/Assertion.h"
#include "Kernel/RenderCameraHelper.h"
#include "Kernel/RenderContextHelper.h"
#include "Kernel/MixinDebug.h"
#include "Kernel/ProfilerHelper.h"
#include "Kernel/ResolutionHelper.h"

#include "Config/StdAlgorithm.h"
#include "Config/StdMath.h"

#include "math/utils.h"

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
            PickerVisitor( VectorPickerStates * _states )
                : m_states( _states )
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
                PickerStateDesc desc;

                desc.picker = _picker;

                desc.context.target = nullptr;
                desc.context.transformation = nullptr;

                const RenderResolutionInterfacePtr & pickerResolution = _picker->getPickerResolution();

                if( pickerResolution != nullptr )
                {
                    desc.context.resolution = pickerResolution.get();
                }
                else
                {
                    desc.context.resolution = _context.resolution;
                }

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

                bool pickerOverChildren = _picker->isPickerOverChildren();

                if( pickerOverChildren == true )
                {
                    _picker->foreachPickerChildrenEnabled( [this, &desc]( PickerInterface * _childPicker )
                    {
                        this->visit( _childPicker, desc.context );
                    } );
                }

                if( _picker->isPickerDummy() == false )
                {
                    m_states->emplace_back( desc );
                }

                if( pickerOverChildren == false )
                {
                    _picker->foreachPickerChildrenEnabled( [this, &desc]( PickerInterface * _childPicker )
                    {
                        this->visit( _childPicker, desc.context );
                    } );
                }
            }

        protected:
            VectorPickerStates * m_states;
        };
        //////////////////////////////////////////////////////////////////////////
        bool hasExclusivePicker( const VectorPickerStates & _states )
        {
            for( const PickerStateDesc & desc : _states )
            {
                if( desc.picker->isPickerExclusive() == true )
                {
                    return true;
                }
            }

            return false;
        }
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

        m_resolution = nullptr;
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
    void PickerService::setRenderResolution( const RenderResolutionInterfacePtr & _resolution )
    {
        if( m_resolution == _resolution )
        {
            return;
        }

        m_resolution = _resolution;

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
        if( this->pickStates_( _point, _touchId, _pressure, _special, &statesAux ) == false )
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
        if( this->getStates_( _point, &statesAux ) == false )
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
    void PickerService::_update()
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
        MENGINE_VECTOR_AUX( m_states );

        ETouchCode touchId = TC_TOUCH0;

        const mt::vec2f & position = INPUT_SERVICE()
            ->getCursorPosition( touchId );

        float pressure = INPUT_SERVICE()
            ->getCursorPressure( touchId );

        InputSpecialData special;
        INPUT_SERVICE()
            ->getSpecial( &special );

        this->pickStates_( position, touchId, pressure, special, &m_states );
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

        if( this->pickStates_( _event.position.screen, TC_TOUCH0, 0.f, _event.special, &m_states ) == false )
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

            if( picker->isPickerFreeze() == true )
            {
                return m_handleValue;
            }

            PickerInputHandlerInterface * inputHandler = picker->getPickerInputHandler();

            InputKeyEvent ne = _event;

            this->calculateInputPositionWorld_( desc, &ne.position );

            LOGGER_INFO( "picker", "handle type '%s' name '%s' UID [%u] pos [%.4f;%.4f] [key]"
                , MENGINE_MIXIN_DEBUG_TYPE( inputHandler )
                , MENGINE_MIXIN_DEBUG_NAME( inputHandler )
                , MENGINE_MIXIN_DEBUG_UID( inputHandler )
                , ne.position.world.x
                , ne.position.world.y
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

        if( this->pickStates_( _event.position.screen, TC_TOUCH0, 0.f, _event.special, &m_states ) == false )
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

            if( picker->isPickerFreeze() == true )
            {
                return m_handleValue;
            }

            PickerInputHandlerInterface * inputHandler = picker->getPickerInputHandler();

            InputTextEvent ne = _event;

            this->calculateInputPositionWorld_( desc, &ne.position );

            LOGGER_INFO( "picker", "handle type '%s' name '%s' UID [%u] pos [%.4f;%.4f] [text]"
                , MENGINE_MIXIN_DEBUG_TYPE( inputHandler )
                , MENGINE_MIXIN_DEBUG_NAME( inputHandler )
                , MENGINE_MIXIN_DEBUG_UID( inputHandler )
                , ne.position.world.x
                , ne.position.world.y
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

        if( this->pickStates_( _event.position.screen, _event.touchId, _event.pressure, _event.special, &m_states ) == false )
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

            if( picker->isPickerFreeze() == true )
            {
                return m_handleValue;
            }

            PickerInputHandlerInterface * inputHandler = picker->getPickerInputHandler();

            InputMouseButtonEvent ne = _event;

            this->calculateInputPositionWorld_( desc, &ne.position );

            ne.isPressed = picker->isPickerPressed();

            LOGGER_INFO( "picker", "handle type '%s' name '%s' UID [%u] pos [%.4f;%.4f] [mouse button]"
                , MENGINE_MIXIN_DEBUG_TYPE( inputHandler )
                , MENGINE_MIXIN_DEBUG_NAME( inputHandler )
                , MENGINE_MIXIN_DEBUG_UID( inputHandler )
                , ne.position.world.x
                , ne.position.world.y
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

        if( this->pickStates_( _event.position.screen, _event.touchId, _event.pressure, _event.special, &m_states ) == false )
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

            if( picker->isPickerFreeze() == true )
            {
                return m_handleValue;
            }

            if( _event.isDown == true )
            {
                picker->setPickerPressed( true );
            }

            PickerInputHandlerInterface * inputHandler = picker->getPickerInputHandler();

            InputMouseButtonEvent ne = _event;

            this->calculateInputPositionWorld_( desc, &ne.position );

            ne.isPressed = picker->isPickerPressed();

            LOGGER_INFO( "picker", "handle type '%s' name '%s' UID [%u] pos [%.4f;%.4f] [mouse button begin]"
                , MENGINE_MIXIN_DEBUG_TYPE( inputHandler )
                , MENGINE_MIXIN_DEBUG_NAME( inputHandler )
                , MENGINE_MIXIN_DEBUG_UID( inputHandler )
                , ne.position.world.x
                , ne.position.world.y
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

        if( this->pickStates_( _event.position.screen, _event.touchId, _event.pressure, _event.special, &m_states ) == false )
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

            if( picker->isPickerFreeze() == true )
            {
                return m_handleValue;
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

            PickerInputHandlerInterface * inputHandler = picker->getPickerInputHandler();

            InputMouseButtonEvent ne = _event;

            this->calculateInputPositionWorld_( desc, &ne.position );

            ne.isPressed = picker->isPickerPressed();

            LOGGER_INFO( "picker", "handle type '%s' name '%s' UID [%u] pos [%.4f;%.4f] [mouse button end]"
                , MENGINE_MIXIN_DEBUG_TYPE( inputHandler )
                , MENGINE_MIXIN_DEBUG_NAME( inputHandler )
                , MENGINE_MIXIN_DEBUG_UID( inputHandler )
                , ne.position.world.x
                , ne.position.world.y
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

        if( this->pickStates_( _event.position.screen, _event.touchId, _event.pressure, _event.special, &m_states ) == false )
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

            if( picker->isPickerFreeze() == true )
            {
                return m_handleValue;
            }

            PickerInputHandlerInterface * inputHandler = picker->getPickerInputHandler();

            InputMouseMoveEvent ne = _event;

            this->calculateInputPositionWorld_( desc, &ne.position );

            ARROW_SERVICE()
                ->calcMouseWorldDelta( &desc.context, ne.screenDelta, &ne.worldDelta );

            LOGGER_INFO( "picker", "handle type '%s' name '%s' UID [%u] pos [%.4f;%.4f] delta [%.4f;%.4f] [mouse move]"
                , MENGINE_MIXIN_DEBUG_TYPE( inputHandler )
                , MENGINE_MIXIN_DEBUG_NAME( inputHandler )
                , MENGINE_MIXIN_DEBUG_UID( inputHandler )
                , ne.position.world.x
                , ne.position.world.y
                , ne.worldDelta.x
                , ne.worldDelta.y
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

        if( this->pickStates_( _event.position.screen, TC_TOUCH0, 0.f, _event.special, &m_states ) == false )
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

            if( picker->isPickerFreeze() == true )
            {
                return m_handleValue;
            }

            PickerInputHandlerInterface * inputHandler = picker->getPickerInputHandler();

            InputMouseWheelEvent ne = _event;

            this->calculateInputPositionWorld_( desc, &ne.position );

            LOGGER_INFO( "picker", "handle type '%s' name '%s' UID [%u] pos [%.4f;%.4f] [mouse wheel]"
                , MENGINE_MIXIN_DEBUG_TYPE( inputHandler )
                , MENGINE_MIXIN_DEBUG_NAME( inputHandler )
                , MENGINE_MIXIN_DEBUG_UID( inputHandler )
                , ne.position.world.x
                , ne.position.world.y
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

        this->pickStates_( _event.position.screen, _event.touchId, _event.pressure, _event.special, &m_states );

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

            if( picker->isPickerFreeze() == true )
            {
                return;
            }

            PickerInputHandlerInterface * inputHandler = picker->getPickerInputHandler();

            InputMouseLeaveEvent ne = _event;

            this->calculateInputPositionWorld_( desc, &ne.position );

            LOGGER_INFO( "picker", "handle type '%s' name '%s' UID [%u] pos [%.4f;%.4f] [mouse leave]"
                , MENGINE_MIXIN_DEBUG_TYPE( inputHandler )
                , MENGINE_MIXIN_DEBUG_NAME( inputHandler )
                , MENGINE_MIXIN_DEBUG_UID( inputHandler )
                , ne.position.world.x
                , ne.position.world.y
            );

            inputHandler->handleMouseLeave( &desc.context, ne );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void PickerService::fillStates_( VectorPickerStates * const _states ) const
    {
        MENGINE_PROFILER_CATEGORY();

        PickerInterface * picker = m_scene->getPicker();

        Detail::PickerVisitor visitor( _states );

        RenderContext context;
        Helper::clearRenderContext( &context );

        context.resolution = m_resolution.get();
        context.viewport = m_viewport.get();
        context.camera = m_camera.get();
        context.transformation = m_transformation.get();
        context.scissor = m_scissor.get();
        context.target = m_target.get();

        visitor.visit( picker, context );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickerService::pickStates_( const mt::vec2f & _screenPosition, ETouchCode _touchId, float _pressure, const InputSpecialData & _special, VectorPickerStates * const _states ) const
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

        mt::vec2f adaptScreenPosition;
        Helper::adaptScreenPosition( _screenPosition, &adaptScreenPosition );

        bool exclusive = Detail::hasExclusivePicker( *_states );

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

            bool pickerExclusive = exclusive == false || picker->isPickerExclusive() == true;
            bool pickerHandle = handle == false || m_handleValue == false;

            bool picked = false;

            if( pickerExclusive == true && pickerHandle == true && this->testPickerScissor_( desc, adaptScreenPosition ) == true )
            {
                picked = picker->pick( adaptScreenPosition, &desc.context );
            }

            if( m_block == false && picked == true )
            {
                if( picker->isPickerPicked() == false )
                {
                    picker->setPickerPicked( true );

                    PickerInputHandlerInterface * inputHandler = picker->getPickerInputHandler();

                    InputMouseEnterEvent ne;

                    ne.special = _special;
                    ne.touchId = _touchId;
                    ne.position.screen = _screenPosition;
                    ne.pressure = _pressure;

                    this->calculateInputPositionWorld_( desc, &ne.position );

                    LOGGER_INFO( "picker", "handle type '%s' name '%s' UID [%u] pos [%.4f;%.4f] [mouse enter]"
                        , MENGINE_MIXIN_DEBUG_TYPE( inputHandler )
                        , MENGINE_MIXIN_DEBUG_NAME( inputHandler )
                        , MENGINE_MIXIN_DEBUG_UID( inputHandler )
                        , ne.position.world.x
                        , ne.position.world.y
                    );

                    handle = inputHandler->handleMouseEnter( &desc.context, ne );

                    picker->setPickerHandle( handle );
                }
                else
                {
                    handle = picker->isPickerHandle();
                }
            }
            else
            {
                if( picker->isPickerPicked() == true )
                {
                    picker->setPickerPicked( false );

                    PickerInputHandlerInterface * inputHandler = picker->getPickerInputHandler();

                    InputMouseLeaveEvent ne;

                    ne.special = _special;
                    ne.touchId = _touchId;
                    ne.position.screen = _screenPosition;
                    ne.pressure = _pressure;

                    this->calculateInputPositionWorld_( desc, &ne.position );

                    LOGGER_INFO( "picker", "handle type '%s' name '%s' UID [%u] pos [%.4f;%.4f] [mouse leave]"
                        , MENGINE_MIXIN_DEBUG_TYPE( inputHandler )
                        , MENGINE_MIXIN_DEBUG_NAME( inputHandler )
                        , MENGINE_MIXIN_DEBUG_UID( inputHandler )
                        , ne.position.world.x
                        , ne.position.world.y
                    );

                    inputHandler->handleMouseLeave( &desc.context, ne );
                }
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickerService::getStates_( const mt::vec2f & _screenPosition, VectorPickerStates * const _states ) const
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

        this->fillStates_( _states );

        mt::vec2f adaptScreenPosition;
        Helper::adaptScreenPosition( _screenPosition, &adaptScreenPosition );

        bool exclusive = Detail::hasExclusivePicker( *_states );

        VectorPickerStates::iterator it_write = _states->begin();

        for( VectorPickerStates::const_iterator
            it = _states->begin(),
            it_end = _states->end();
            it != it_end;
            ++it )
        {
            const PickerStateDesc & desc = *it;

            PickerInterface * picker = desc.picker;

            if( picker->isPickerEnable() == false )
            {
                continue;
            }

            if( exclusive == true && picker->isPickerExclusive() == false )
            {
                continue;
            }

            if( this->testPickerScissor_( desc, adaptScreenPosition ) == false )
            {
                continue;
            }

            if( picker->pick( adaptScreenPosition, &desc.context ) == false )
            {
                continue;
            }

            if( it_write != it )
            {
                *it_write = desc;
            }

            ++it_write;
        }

        _states->erase( it_write, _states->end() );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickerService::testPickerScissor_( const PickerStateDesc & _desc, const mt::vec2f & _screenPosition ) const
    {
        const RenderScissorInterface * scissor = _desc.context.scissor;

        if( scissor == nullptr )
        {
            return true;
        }

        const RenderResolutionInterface * resolution = _desc.context.resolution;

        if( resolution == nullptr )
        {
            return false;
        }

        const Viewport & scissorViewport = scissor->getScissorViewportWM();

        mt::vec2f contentPosition;
        resolution->fromScreenToContentPosition( _screenPosition, &contentPosition );

        EArrowType arrowType = ARROW_SERVICE()
            ->getArrowType();

        switch( arrowType )
        {
        case EAT_POINT:
            {
                return scissorViewport.testPoint( contentPosition );
            }break;
        case EAT_RADIUS:
            {
                if( _desc.context.camera == nullptr || _desc.context.viewport == nullptr )
                {
                    return false;
                }

                float radius = ARROW_SERVICE()
                    ->getArrowTypeRadius();

                mt::vec2f screenRadius;
                Helper::worldToScreenDelta( &_desc.context, mt::vec2f( radius, radius ), &screenRadius );

                mt::vec2f contentRadius;
                resolution->fromScreenToContentPosition( screenRadius, &contentRadius );

                contentRadius.x = mt::abs_f( contentRadius.x );
                contentRadius.y = mt::abs_f( contentRadius.y );

                return scissorViewport.testRectangle( contentPosition - contentRadius, contentPosition + contentRadius );
            }break;
        case EAT_POLYGON:
            {
                const Polygon & polygon = ARROW_SERVICE()
                    ->getArrowTypePolygon();

                if( polygon.empty() == true )
                {
                    return false;
                }

                mt::box2f polygonBox;
                polygon.to_box2f( &polygonBox );

                mt::vec2f screenMinimum = _screenPosition + polygonBox.minimum;
                mt::vec2f screenMaximum = _screenPosition + polygonBox.maximum;

                mt::vec2f contentMinimum;
                resolution->fromScreenToContentPosition( screenMinimum, &contentMinimum );

                mt::vec2f contentMaximum;
                resolution->fromScreenToContentPosition( screenMaximum, &contentMaximum );

                return scissorViewport.testRectangle( contentMinimum, contentMaximum );
            }break;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void PickerService::calculateInputPositionWorld_( const PickerStateDesc & _desc, InputPositionData * const _position ) const
    {
        ARROW_SERVICE()
            ->calcMouseWorldPosition( &_desc.context, _position->screen, &_position->world );
    }
    //////////////////////////////////////////////////////////////////////////
}
