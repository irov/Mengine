#include "MCPInputMouseCommand.h"

#include "../../Contexts/MCPInputContext.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/InputServiceHelper.h"
#include "Kernel/Pair.h"
#include "Kernel/TimestampHelper.h"

#include "Config/Limits.h"
#include "Config/StdMath.h"
#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        typedef Pair<const Char *, EInputGesturePhase> PairGesturePhase;
        typedef Pair<const Char *, EInputScrollUnit> PairScrollUnit;
        //////////////////////////////////////////////////////////////////////////
        static const PairGesturePhase GESTURE_PHASES[] = {
            {"none", IGP_NONE},
            {"may_begin", IGP_MAY_BEGIN},
            {"begin", IGP_BEGIN},
            {"change", IGP_CHANGE},
            {"end", IGP_END},
            {"cancel", IGP_CANCEL}
        };
        static const PairScrollUnit SCROLL_UNITS[] = {
            {"steps", ISU_STEPS},
            {"screen", ISU_SCREEN},
            {"lines", ISU_LINES},
            {"pages", ISU_PAGES}
        };
        //////////////////////////////////////////////////////////////////////////
        template<class T, size_t N>
        static bool readMouseGestureEnum( const jpp::object & _params, const Char * _key, const T (&_values)[N], typename T::second_type * const _value )
        {
            jpp::object value;
            if( _params.exist( _key, &value ) == false )
            {
                return true;
            }

            if( value.is_type_string() == false )
            {
                return false;
            }

            const Char * name = value;

            for( const T & desc : _values )
            {
                if( StdString::strcmp( name, desc.first ) == 0 )
                {
                    *_value = desc.second;

                    return true;
                }
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool readMouseGestureNumber( const jpp::object & _params, const Char * _key, float * const _value )
        {
            jpp::object value;
            if( _params.exist( _key, &value ) == false )
            {
                return true;
            }

            if( value.is_type_integer() == false && value.is_type_real() == false )
            {
                return false;
            }

            double number = value;
            double magnitude = StdMath::fabs( number );

            if( (magnitude <= MENGINE_FLT_MAX) == false )
            {
                return false;
            }

            *_value = (float)number;

            return true;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    MCPInputMouseCommand::MCPInputMouseCommand( MCPInputContext * _context, MCPSceneContext * _sceneContext )
        : m_context( _context )
        , m_sceneContext( _sceneContext )
    {
        ConstString move = STRINGIZE_STRING_LOCAL( "move" );
        ConstString down = STRINGIZE_STRING_LOCAL( "down" );
        ConstString up = STRINGIZE_STRING_LOCAL( "up" );
        ConstString click = STRINGIZE_STRING_LOCAL( "click" );
        ConstString doubleClick = STRINGIZE_STRING_LOCAL( "double_click" );
        ConstString wheel = STRINGIZE_STRING_LOCAL( "wheel" );
        ConstString scroll = STRINGIZE_STRING_LOCAL( "scroll" );
        ConstString pinch = STRINGIZE_STRING_LOCAL( "pinch" );
        ConstString rotate = STRINGIZE_STRING_LOCAL( "rotate" );
        ConstString smartMagnify = STRINGIZE_STRING_LOCAL( "smart_magnify" );
        ConstString swipe = STRINGIZE_STRING_LOCAL( "swipe" );
        ConstString pressure = STRINGIZE_STRING_LOCAL( "pressure" );

        m_actions.emplace( move, &MCPInputMouseCommand::move_ );
        m_actions.emplace( down, &MCPInputMouseCommand::down_ );
        m_actions.emplace( up, &MCPInputMouseCommand::up_ );
        m_actions.emplace( click, &MCPInputMouseCommand::click_ );
        m_actions.emplace( doubleClick, &MCPInputMouseCommand::doubleClick_ );
        m_actions.emplace( wheel, &MCPInputMouseCommand::wheel_ );
        m_actions.emplace( scroll, &MCPInputMouseCommand::scroll_ );
        m_actions.emplace( pinch, &MCPInputMouseCommand::pinch_ );
        m_actions.emplace( rotate, &MCPInputMouseCommand::rotate_ );
        m_actions.emplace( smartMagnify, &MCPInputMouseCommand::smartMagnify_ );
        m_actions.emplace( swipe, &MCPInputMouseCommand::swipe_ );
        m_actions.emplace( pressure, &MCPInputMouseCommand::pressure_ );
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPInputMouseCommand::execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        const Char * actionValue = _request.params.get( "action", "" );
        ConstString actionName = Helper::stringizeString( actionValue );
        MapActions::const_iterator it = m_actions.find( actionName );

        if( it == m_actions.end() )
        {
            _response->errorMessage = "unknown mouse action";

            return EMCPCommandStatus::FAILURE;
        }

        const Char * coordinateSpace = _request.params.get( "coordinateSpace", "normalized" );

        mt::vec2f point;
        String error;
        if( m_context->resolvePoint( m_sceneContext, _request.params, coordinateSpace, &point, &error ) == false )
        {
            _response->errorMessage = error;

            return EMCPCommandStatus::FAILURE;
        }

        const Char * buttonValue = _request.params.get( "button", "left" );

        EMouseButtonCode button;
        if( m_context->getMouseButton( buttonValue, &button ) == false )
        {
            _response->errorMessage = "unknown mouse button, expected left, middle, right, x1 or x2";

            return EMCPCommandStatus::FAILURE;
        }

        VectorMCPKeyCodes modifiers;
        if( m_context->getModifierKeys( _request.params, &modifiers, &error ) == false )
        {
            _response->errorMessage = error;

            return EMCPCommandStatus::FAILURE;
        }

        float previousX = m_context->getCursorX();
        float previousY = m_context->getCursorY();

        MouseEventDesc event{};
        m_context->getSpecial( modifiers, &event.special );

        event.timestamp = Helper::getSystemTimestamp();
        event.x = point.x;
        event.y = point.y;
        event.dx = point.x - previousX;
        event.dy = point.y - previousY;
        event.button = button;
        event.wheel = _request.params.get( "wheel", 0 );

        Action action = it->second;

        if( action == &MCPInputMouseCommand::scroll_ )
        {
            InputScrollData & data = event.scroll;
            data.delta.set( 0.f, 0.f );
            data.unit = ISU_SCREEN;

            if( Detail::readMouseGestureNumber( _request.params, "deltaX", &data.delta.x ) == false )
            {
                _response->errorMessage = "scroll deltas must be finite numbers";

                return EMCPCommandStatus::FAILURE;
            }

            if( Detail::readMouseGestureNumber( _request.params, "deltaY", &data.delta.y ) == false )
            {
                _response->errorMessage = "scroll deltas must be finite numbers";

                return EMCPCommandStatus::FAILURE;
            }

            if( Detail::readMouseGestureEnum( _request.params, "unit", Detail::SCROLL_UNITS, &data.unit ) == false )
            {
                _response->errorMessage = "invalid scroll unit or phase";

                return EMCPCommandStatus::FAILURE;
            }

            if( Detail::readMouseGestureEnum( _request.params, "phase", Detail::GESTURE_PHASES, &data.phase ) == false )
            {
                _response->errorMessage = "invalid scroll unit or phase";

                return EMCPCommandStatus::FAILURE;
            }

            if( Detail::readMouseGestureEnum( _request.params, "momentumPhase", Detail::GESTURE_PHASES, &data.momentumPhase ) == false )
            {
                _response->errorMessage = "invalid scroll unit or phase";

                return EMCPCommandStatus::FAILURE;
            }

            data.isPrecise = data.unit == ISU_SCREEN;

            if( data.isPrecise == false && data.phase != IGP_CANCEL )
            {
                data.steps = data.delta.y > 0.f ? -1 : (data.delta.y < 0.f ? 1 : 0);
            }

            jpp::object inverted;
            if( _request.params.exist( "directionInverted", &inverted ) == true )
            {
                if( inverted.is_true() == false && inverted.is_false() == false )
                {
                    _response->errorMessage = "directionInverted must be a boolean";

                    return EMCPCommandStatus::FAILURE;
                }

                data.isDirectionInvertedFromDevice = inverted.is_true();
            }
        }

        if( action == &MCPInputMouseCommand::pinch_ || action == &MCPInputMouseCommand::rotate_ || action == &MCPInputMouseCommand::pressure_ )
        {
            if( Detail::readMouseGestureEnum( _request.params, "phase", Detail::GESTURE_PHASES, &event.gesturePhase ) == false )
            {
                _response->errorMessage = "invalid gesture phase";

                return EMCPCommandStatus::FAILURE;
            }
        }

        if( action == &MCPInputMouseCommand::pinch_ )
        {
            if( Detail::readMouseGestureNumber( _request.params, "scale", &event.scale ) == false )
            {
                _response->errorMessage = "pinch scale must be a positive finite number";

                return EMCPCommandStatus::FAILURE;
            }

            if( event.scale <= 0.f )
            {
                _response->errorMessage = "pinch scale must be a positive finite number";

                return EMCPCommandStatus::FAILURE;
            }
        }
        else if( action == &MCPInputMouseCommand::rotate_ )
        {
            if( Detail::readMouseGestureNumber( _request.params, "degrees", &event.degrees ) == false )
            {
                _response->errorMessage = "rotation degrees must be a finite number";

                return EMCPCommandStatus::FAILURE;
            }
        }
        else if( action == &MCPInputMouseCommand::swipe_ )
        {
            mt::vec2f & direction = event.swipeDirection;

            if( Detail::readMouseGestureNumber( _request.params, "deltaX", &direction.x ) == false )
            {
                _response->errorMessage = "swipe deltaX must be between -1 and 1";

                return EMCPCommandStatus::FAILURE;
            }

            if( direction.x < -1.f )
            {
                _response->errorMessage = "swipe deltaX must be between -1 and 1";

                return EMCPCommandStatus::FAILURE;
            }

            if( direction.x > 1.f )
            {
                _response->errorMessage = "swipe deltaX must be between -1 and 1";

                return EMCPCommandStatus::FAILURE;
            }

            if( Detail::readMouseGestureNumber( _request.params, "deltaY", &direction.y ) == false )
            {
                _response->errorMessage = "swipe deltaY must be between -1 and 1";

                return EMCPCommandStatus::FAILURE;
            }

            if( direction.y < -1.f )
            {
                _response->errorMessage = "swipe deltaY must be between -1 and 1";

                return EMCPCommandStatus::FAILURE;
            }

            if( direction.y > 1.f )
            {
                _response->errorMessage = "swipe deltaY must be between -1 and 1";

                return EMCPCommandStatus::FAILURE;
            }

            if( direction.x == 0.f && direction.y == 0.f )
            {
                _response->errorMessage = "swipe direction must be nonzero";

                return EMCPCommandStatus::FAILURE;
            }
        }
        else if( action == &MCPInputMouseCommand::pressure_ )
        {
            if( Detail::readMouseGestureNumber( _request.params, "pressure", &event.pressure ) == false )
            {
                _response->errorMessage = "pressure must be between 0 and 1";

                return EMCPCommandStatus::FAILURE;
            }

            if( event.pressure < 0.f )
            {
                _response->errorMessage = "pressure must be between 0 and 1";

                return EMCPCommandStatus::FAILURE;
            }

            if( event.pressure > 1.f )
            {
                _response->errorMessage = "pressure must be between 0 and 1";

                return EMCPCommandStatus::FAILURE;
            }

            if( Detail::readMouseGestureNumber( _request.params, "stageTransition", &event.stageTransition ) == false )
            {
                _response->errorMessage = "stageTransition must be between -1 and 1";

                return EMCPCommandStatus::FAILURE;
            }

            if( event.stageTransition < -1.f )
            {
                _response->errorMessage = "stageTransition must be between -1 and 1";

                return EMCPCommandStatus::FAILURE;
            }

            if( event.stageTransition > 1.f )
            {
                _response->errorMessage = "stageTransition must be between -1 and 1";

                return EMCPCommandStatus::FAILURE;
            }

            jpp::object stageValue;
            if( _request.params.exist( "stage", &stageValue ) == true )
            {
                if( stageValue.is_type_integer() == false )
                {
                    _response->errorMessage = "pressure stage must be an integer";

                    return EMCPCommandStatus::FAILURE;
                }

                int64_t stage = stageValue;

                if( stage < 0 )
                {
                    _response->errorMessage = "pressure stage is out of range";

                    return EMCPCommandStatus::FAILURE;
                }

                if( stage > INT32_MAX )
                {
                    _response->errorMessage = "pressure stage is out of range";

                    return EMCPCommandStatus::FAILURE;
                }

                event.pressureStage = (int32_t)stage;
            }
        }

        m_context->setCursor( point.x, point.y );

        if( action != &MCPInputMouseCommand::up_ )
        {
            m_context->pushModifierKeys( modifiers, true );
        }

        (this->*action)( event );

        if( action != &MCPInputMouseCommand::down_ )
        {
            m_context->pushModifierKeys( modifiers, false );
        }

        _response->result.set( "action", actionValue );
        _response->result.set( "x", point.x );
        _response->result.set( "y", point.y );

        return EMCPCommandStatus::SUCCESS;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPInputMouseCommand::move_( const MouseEventDesc & _event )
    {
        Helper::pushMouseMoveEvent( _event.timestamp, TC_TOUCH0, _event.x, _event.y, _event.dx, _event.dy, 0.f, 0.f, _event.special );
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPInputMouseCommand::down_( const MouseEventDesc & _event )
    {
        Helper::pushMouseButtonEvent( _event.timestamp, TC_TOUCH0, _event.x, _event.y, _event.button, 1.f, true, _event.special );
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPInputMouseCommand::up_( const MouseEventDesc & _event )
    {
        Helper::pushMouseButtonEvent( _event.timestamp, TC_TOUCH0, _event.x, _event.y, _event.button, 0.f, false, _event.special );
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPInputMouseCommand::click_( const MouseEventDesc & _event )
    {
        this->down_( _event );
        this->up_( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPInputMouseCommand::doubleClick_( const MouseEventDesc & _event )
    {
        this->click_( _event );
        this->click_( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPInputMouseCommand::wheel_( const MouseEventDesc & _event )
    {
        Helper::pushMouseWheelEvent( _event.timestamp, _event.x, _event.y, 0.f, WC_CENTRAL, _event.wheel, _event.special );
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPInputMouseCommand::scroll_( const MouseEventDesc & _event )
    {
        Helper::pushMouseWheelEvent( _event.timestamp, _event.x, _event.y, 0.f, WC_CENTRAL, _event.scroll, _event.special );
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPInputMouseCommand::pinch_( const MouseEventDesc & _event )
    {
        Helper::pushGestureMagnificationEvent( _event.timestamp, _event.x, _event.y, _event.scale, _event.gesturePhase, _event.special );
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPInputMouseCommand::rotate_( const MouseEventDesc & _event )
    {
        Helper::pushGestureRotationEvent( _event.timestamp, _event.x, _event.y, _event.degrees, _event.gesturePhase, _event.special );
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPInputMouseCommand::smartMagnify_( const MouseEventDesc & _event )
    {
        Helper::pushGestureSmartMagnificationEvent( _event.timestamp, _event.x, _event.y, _event.special );
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPInputMouseCommand::swipe_( const MouseEventDesc & _event )
    {
        Helper::pushGestureSwipeEvent( _event.timestamp, _event.x, _event.y, _event.swipeDirection, _event.special );
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPInputMouseCommand::pressure_( const MouseEventDesc & _event )
    {
        Helper::pushGesturePressureEvent( _event.timestamp, _event.x, _event.y, _event.pressure, _event.pressureStage, _event.stageTransition, _event.gesturePhase, _event.special );
    }
    //////////////////////////////////////////////////////////////////////////
}
