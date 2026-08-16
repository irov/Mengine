#pragma once

#include "Interface/ActionServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    class ActionService
        : public ServiceBase<ActionServiceInterface>
    {
        DECLARE_FACTORABLE( ActionService );
        DECLARE_LIFECYCLEABLE();

    public:
        ActionService();
        ~ActionService() override;

    public:
        const ServiceRequiredList & requiredServices() const override;
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        void _postUpdate() override;

    public:
        bool addAction( const ConstString & _action, float _deadzone ) override;
        bool removeAction( const ConstString & _action ) override;
        bool hasAction( const ConstString & _action ) const override;
        bool setActionDeadzone( const ConstString & _action, float _deadzone ) override;
        float getActionDeadzone( const ConstString & _action ) const override;

    public:
        bool addKeyBinding( const ConstString & _action, EKeyCode _keyCode, float _strength ) override;
        bool removeKeyBinding( const ConstString & _action, EKeyCode _keyCode ) override;
        bool addMouseButtonBinding( const ConstString & _action, EMouseButtonCode _button, float _strength ) override;
        bool removeMouseButtonBinding( const ConstString & _action, EMouseButtonCode _button ) override;
        bool clearBindings( const ConstString & _action ) override;

    public:
        bool setActionStrength( const ConstString & _action, float _strength ) override;
        bool releaseAction( const ConstString & _action ) override;

    public:
        bool isActionPressed( const ConstString & _action ) const override;
        bool isActionJustPressed( const ConstString & _action ) const override;
        bool isActionJustReleased( const ConstString & _action ) const override;
        float getActionStrength( const ConstString & _action ) const override;
        float getActionAxis( const ConstString & _negativeAction, const ConstString & _positiveAction ) const override;
        void calcActionVector( const ConstString & _negativeX, const ConstString & _positiveX, const ConstString & _negativeY, const ConstString & _positiveY, float _deadzone, mt::vec2f * const _value ) const override;

    public:
        void handleFocus( bool _focus ) override;
        bool handleKeyEvent( const InputKeyEvent & _event ) override;
        bool handleTextEvent( const InputTextEvent & _event ) override;
        bool handleAccelerometerEvent( const InputAccelerometerEvent & _event ) override;
        bool handleMouseButtonEvent( const InputMouseButtonEvent & _event ) override;
        bool handleMouseButtonEventBegin( const InputMouseButtonEvent & _event ) override;
        bool handleMouseButtonEventEnd( const InputMouseButtonEvent & _event ) override;
        bool handleMouseMove( const InputMouseMoveEvent & _event ) override;
        bool handleMouseWheel( const InputMouseWheelEvent & _event ) override;
        bool handleMouseEnter( const InputMouseEnterEvent & _event ) override;
        void handleMouseLeave( const InputMouseLeaveEvent & _event ) override;

    protected:
        enum class EBindingType
        {
            Key,
            MouseButton
        };

        struct BindingDesc
        {
            EBindingType type;
            uint32_t code;
            float strength;
            bool active;
        };

        typedef Vector<BindingDesc> VectorBindings;

        struct ActionDesc
        {
            ConstString name;
            VectorBindings bindings;
            float deadzone;
            float virtualStrength;
            float strength;
            bool pressed;
            bool justPressed;
            bool justReleased;
        };

        typedef Vector<ActionDesc> VectorActions;

    protected:
        ActionDesc * findAction_( const ConstString & _action );
        const ActionDesc * findAction_( const ConstString & _action ) const;
        bool addBinding_( const ConstString & _action, EBindingType _type, uint32_t _code, float _strength, bool _active );
        bool removeBinding_( const ConstString & _action, EBindingType _type, uint32_t _code );
        void updateActionState_( ActionDesc * const _action );
        float getVectorDeadzone_( const ConstString & _negativeX, const ConstString & _positiveX, const ConstString & _negativeY, const ConstString & _positiveY ) const;

    protected:
        VectorActions m_actions;
    };
}
