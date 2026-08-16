#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/InputHandlerInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/KeyCode.h"
#include "Kernel/MouseButtonCode.h"

#include "math/vec2.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ActionServiceInterface
        : public ServiceInterface
        , public InputHandlerInterface
    {
        SERVICE_DECLARE( "ActionService" )

    public:
        virtual bool addAction( const ConstString & _action, float _deadzone ) = 0;
        virtual bool removeAction( const ConstString & _action ) = 0;
        virtual bool hasAction( const ConstString & _action ) const = 0;
        virtual bool setActionDeadzone( const ConstString & _action, float _deadzone ) = 0;
        virtual float getActionDeadzone( const ConstString & _action ) const = 0;

    public:
        virtual bool addKeyBinding( const ConstString & _action, EKeyCode _keyCode, float _strength ) = 0;
        virtual bool removeKeyBinding( const ConstString & _action, EKeyCode _keyCode ) = 0;
        virtual bool addMouseButtonBinding( const ConstString & _action, EMouseButtonCode _button, float _strength ) = 0;
        virtual bool removeMouseButtonBinding( const ConstString & _action, EMouseButtonCode _button ) = 0;
        virtual bool clearBindings( const ConstString & _action ) = 0;

    public:
        virtual bool setActionStrength( const ConstString & _action, float _strength ) = 0;
        virtual bool releaseAction( const ConstString & _action ) = 0;

    public:
        virtual bool isActionPressed( const ConstString & _action ) const = 0;
        virtual bool isActionJustPressed( const ConstString & _action ) const = 0;
        virtual bool isActionJustReleased( const ConstString & _action ) const = 0;
        virtual float getActionStrength( const ConstString & _action ) const = 0;
        virtual float getActionAxis( const ConstString & _negativeAction, const ConstString & _positiveAction ) const = 0;
        virtual void calcActionVector( const ConstString & _negativeX, const ConstString & _positiveX, const ConstString & _negativeY, const ConstString & _positiveY, float _deadzone, mt::vec2f * const _value ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define ACTION_SERVICE()\
    ((Mengine::ActionServiceInterface *)SERVICE_GET(Mengine::ActionServiceInterface))
//////////////////////////////////////////////////////////////////////////
