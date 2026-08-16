#include "ActionService.h"

#include "Interface/InputServiceInterface.h"

#include "Kernel/Assertion.h"

#include "Config/StdAlgorithm.h"

#include "math/utils.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ActionService, Mengine::ActionService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    ActionService::ActionService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ActionService::~ActionService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const ServiceRequiredList & ActionService::requiredServices() const
    {
        static ServiceRequiredList required = {
            SERVICE_ID( InputServiceInterface )
        };

        return required;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ActionService::_initializeService()
    {
        m_actions.reserve( 32 );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ActionService::_finalizeService()
    {
        m_actions.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void ActionService::_postUpdate()
    {
        for( ActionDesc & action : m_actions )
        {
            action.justPressed = false;
            action.justReleased = false;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool ActionService::addAction( const ConstString & _action, float _deadzone )
    {
        if( _action.empty() == true || this->hasAction( _action ) == true )
        {
            return false;
        }

        ActionDesc desc;
        desc.name = _action;
        desc.deadzone = StdAlgorithm::clamp( _deadzone, 0.f, 1.f );
        desc.virtualStrength = 0.f;
        desc.strength = 0.f;
        desc.pressed = false;
        desc.justPressed = false;
        desc.justReleased = false;

        m_actions.emplace_back( desc );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ActionService::removeAction( const ConstString & _action )
    {
        VectorActions::iterator it_found = StdAlgorithm::find_if( m_actions.begin(), m_actions.end(), [_action]( const ActionDesc & _desc )
        {
            return _desc.name == _action;
        } );

        if( it_found == m_actions.end() )
        {
            return false;
        }

        m_actions.erase( it_found );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ActionService::hasAction( const ConstString & _action ) const
    {
        const ActionDesc * desc = this->findAction_( _action );

        bool exist = desc != nullptr;

        return exist;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ActionService::setActionDeadzone( const ConstString & _action, float _deadzone )
    {
        ActionDesc * desc = this->findAction_( _action );

        if( desc == nullptr )
        {
            return false;
        }

        desc->deadzone = StdAlgorithm::clamp( _deadzone, 0.f, 1.f );
        this->updateActionState_( desc );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    float ActionService::getActionDeadzone( const ConstString & _action ) const
    {
        const ActionDesc * desc = this->findAction_( _action );

        if( desc == nullptr )
        {
            return 0.f;
        }

        return desc->deadzone;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ActionService::addKeyBinding( const ConstString & _action, EKeyCode _keyCode, float _strength )
    {
        bool active = INPUT_SERVICE()->isKeyDown( _keyCode );

        bool successful = this->addBinding_( _action, EBindingType::Key, (uint32_t)_keyCode, _strength, active );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ActionService::removeKeyBinding( const ConstString & _action, EKeyCode _keyCode )
    {
        bool successful = this->removeBinding_( _action, EBindingType::Key, (uint32_t)_keyCode );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ActionService::addMouseButtonBinding( const ConstString & _action, EMouseButtonCode _button, float _strength )
    {
        MENGINE_ASSERTION_FATAL( _button < MENGINE_INPUT_MAX_MOUSE_BUTTON_CODE, "invalid mouse button code %u"
            , _button
        );

        bool active = INPUT_SERVICE()->isMouseButtonDown( _button );

        bool successful = this->addBinding_( _action, EBindingType::MouseButton, (uint32_t)_button, _strength, active );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ActionService::removeMouseButtonBinding( const ConstString & _action, EMouseButtonCode _button )
    {
        bool successful = this->removeBinding_( _action, EBindingType::MouseButton, (uint32_t)_button );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ActionService::clearBindings( const ConstString & _action )
    {
        ActionDesc * desc = this->findAction_( _action );

        if( desc == nullptr )
        {
            return false;
        }

        desc->bindings.clear();
        this->updateActionState_( desc );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ActionService::setActionStrength( const ConstString & _action, float _strength )
    {
        ActionDesc * desc = this->findAction_( _action );

        if( desc == nullptr )
        {
            return false;
        }

        desc->virtualStrength = StdAlgorithm::clamp( _strength, 0.f, 1.f );
        this->updateActionState_( desc );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ActionService::releaseAction( const ConstString & _action )
    {
        bool successful = this->setActionStrength( _action, 0.f );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ActionService::isActionPressed( const ConstString & _action ) const
    {
        const ActionDesc * desc = this->findAction_( _action );

        return desc != nullptr ? desc->pressed : false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ActionService::isActionJustPressed( const ConstString & _action ) const
    {
        const ActionDesc * desc = this->findAction_( _action );

        return desc != nullptr ? desc->justPressed : false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ActionService::isActionJustReleased( const ConstString & _action ) const
    {
        const ActionDesc * desc = this->findAction_( _action );

        return desc != nullptr ? desc->justReleased : false;
    }
    //////////////////////////////////////////////////////////////////////////
    float ActionService::getActionStrength( const ConstString & _action ) const
    {
        const ActionDesc * desc = this->findAction_( _action );

        return desc != nullptr ? desc->strength : 0.f;
    }
    //////////////////////////////////////////////////////////////////////////
    float ActionService::getActionAxis( const ConstString & _negativeAction, const ConstString & _positiveAction ) const
    {
        float negativeStrength = this->getActionStrength( _negativeAction );
        float positiveStrength = this->getActionStrength( _positiveAction );

        return positiveStrength - negativeStrength;
    }
    //////////////////////////////////////////////////////////////////////////
    void ActionService::calcActionVector( const ConstString & _negativeX, const ConstString & _positiveX, const ConstString & _negativeY, const ConstString & _positiveY, float _deadzone, mt::vec2f * const _value ) const
    {
        mt::vec2f value( this->getActionAxis( _negativeX, _positiveX ), this->getActionAxis( _negativeY, _positiveY ) );

        float length = mt::length_v2( value );

        if( length > 1.f )
        {
            value /= length;
            length = 1.f;
        }

        float deadzone = _deadzone;

        if( deadzone < 0.f )
        {
            deadzone = this->getVectorDeadzone_( _negativeX, _positiveX, _negativeY, _positiveY );
        }
        else
        {
            deadzone = StdAlgorithm::clamp( deadzone, 0.f, 1.f );
        }

        if( length <= deadzone || length == 0.f )
        {
            *_value = mt::vec2f( 0.f, 0.f );

            return;
        }

        if( deadzone >= 1.f )
        {
            *_value = value;

            return;
        }

        float normalizedLength = (length - deadzone) / (1.f - deadzone);

        value *= normalizedLength / length;

        *_value = value;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ActionService::handleKeyEvent( const InputKeyEvent & _event )
    {
        for( ActionDesc & action : m_actions )
        {
            bool changed = false;

            for( BindingDesc & binding : action.bindings )
            {
                if( binding.type != EBindingType::Key || binding.code != (uint32_t)_event.code )
                {
                    continue;
                }

                if( binding.active == _event.isDown )
                {
                    continue;
                }

                binding.active = _event.isDown;
                changed = true;
            }

            if( changed == true )
            {
                this->updateActionState_( &action );
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ActionService::handleTextEvent( const InputTextEvent & _event )
    {
        MENGINE_UNUSED( _event );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ActionService::handleAccelerometerEvent( const InputAccelerometerEvent & _event )
    {
        MENGINE_UNUSED( _event );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ActionService::handleMouseButtonEvent( const InputMouseButtonEvent & _event )
    {
        for( ActionDesc & action : m_actions )
        {
            bool changed = false;

            for( BindingDesc & binding : action.bindings )
            {
                if( binding.type != EBindingType::MouseButton || binding.code != (uint32_t)_event.button )
                {
                    continue;
                }

                if( binding.active == _event.isDown )
                {
                    continue;
                }

                binding.active = _event.isDown;
                changed = true;
            }

            if( changed == true )
            {
                this->updateActionState_( &action );
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ActionService::handleMouseButtonEventBegin( const InputMouseButtonEvent & _event )
    {
        MENGINE_UNUSED( _event );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ActionService::handleMouseButtonEventEnd( const InputMouseButtonEvent & _event )
    {
        MENGINE_UNUSED( _event );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ActionService::handleMouseMove( const InputMouseMoveEvent & _event )
    {
        MENGINE_UNUSED( _event );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ActionService::handleMouseWheel( const InputMouseWheelEvent & _event )
    {
        MENGINE_UNUSED( _event );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ActionService::handleMouseEnter( const InputMouseEnterEvent & _event )
    {
        MENGINE_UNUSED( _event );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void ActionService::handleMouseLeave( const InputMouseLeaveEvent & _event )
    {
        MENGINE_UNUSED( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void ActionService::handleFocus( bool _focus )
    {
        if( _focus == true )
        {
            return;
        }

        for( ActionDesc & action : m_actions )
        {
            for( BindingDesc & binding : action.bindings )
            {
                binding.active = false;
            }

            this->updateActionState_( &action );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    ActionService::ActionDesc * ActionService::findAction_( const ConstString & _action )
    {
        VectorActions::iterator it_found = StdAlgorithm::find_if( m_actions.begin(), m_actions.end(), [_action]( const ActionDesc & _desc )
        {
            return _desc.name == _action;
        } );

        VectorActions::iterator it_end = m_actions.end();
        ActionDesc * desc = it_found != it_end ? &*it_found : nullptr;

        return desc;
    }
    //////////////////////////////////////////////////////////////////////////
    const ActionService::ActionDesc * ActionService::findAction_( const ConstString & _action ) const
    {
        VectorActions::const_iterator it_found = StdAlgorithm::find_if( m_actions.begin(), m_actions.end(), [_action]( const ActionDesc & _desc )
        {
            return _desc.name == _action;
        } );

        VectorActions::const_iterator it_end = m_actions.end();
        const ActionDesc * desc = it_found != it_end ? &*it_found : nullptr;

        return desc;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ActionService::addBinding_( const ConstString & _action, EBindingType _type, uint32_t _code, float _strength, bool _active )
    {
        ActionDesc * desc = this->findAction_( _action );

        if( desc == nullptr )
        {
            return false;
        }

        VectorBindings::const_iterator it_found = StdAlgorithm::find_if( desc->bindings.begin(), desc->bindings.end(), [_type, _code]( const BindingDesc & _binding )
        {
            return _binding.type == _type && _binding.code == _code;
        } );

        if( it_found != desc->bindings.end() )
        {
            return false;
        }

        BindingDesc binding;
        binding.type = _type;
        binding.code = _code;
        binding.strength = StdAlgorithm::clamp( _strength, 0.f, 1.f );
        binding.active = _active;

        desc->bindings.emplace_back( binding );
        this->updateActionState_( desc );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ActionService::removeBinding_( const ConstString & _action, EBindingType _type, uint32_t _code )
    {
        ActionDesc * desc = this->findAction_( _action );

        if( desc == nullptr )
        {
            return false;
        }

        VectorBindings::iterator it_found = StdAlgorithm::find_if( desc->bindings.begin(), desc->bindings.end(), [_type, _code]( const BindingDesc & _binding )
        {
            return _binding.type == _type && _binding.code == _code;
        } );

        if( it_found == desc->bindings.end() )
        {
            return false;
        }

        desc->bindings.erase( it_found );
        this->updateActionState_( desc );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ActionService::updateActionState_( ActionDesc * const _action )
    {
        float strength = _action->virtualStrength;

        for( const BindingDesc & binding : _action->bindings )
        {
            if( binding.active == false )
            {
                continue;
            }

            strength = StdAlgorithm::max( strength, binding.strength );
        }

        bool pressed = strength > _action->deadzone;

        if( pressed == true && _action->pressed == false )
        {
            _action->justPressed = true;
        }
        else if( pressed == false && _action->pressed == true )
        {
            _action->justReleased = true;
        }

        _action->strength = strength;
        _action->pressed = pressed;
    }
    //////////////////////////////////////////////////////////////////////////
    float ActionService::getVectorDeadzone_( const ConstString & _negativeX, const ConstString & _positiveX, const ConstString & _negativeY, const ConstString & _positiveY ) const
    {
        float deadzone = 0.f;
        uint32_t actionCount = 0;

        const ConstString actions[] = {_negativeX, _positiveX, _negativeY, _positiveY};

        for( const ConstString & action : actions )
        {
            const ActionDesc * desc = this->findAction_( action );

            if( desc == nullptr )
            {
                continue;
            }

            deadzone += desc->deadzone;
            ++actionCount;
        }

        if( actionCount == 0 )
        {
            return 0.f;
        }

        float averageDeadzone = deadzone / (float)actionCount;

        return averageDeadzone;
    }
    //////////////////////////////////////////////////////////////////////////
}
