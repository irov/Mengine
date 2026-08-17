#pragma once

#include "UIControlCallbackInterface.h"

#include "Interface/InputControllerHandlerInterface.h"

#include "Kernel/Node.h"
#include "Kernel/BaseTransformation.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EUIFocusDirection
    {
        UIFD_LEFT,
        UIFD_RIGHT,
        UIFD_UP,
        UIFD_DOWN,

        __UI_FOCUS_DIRECTION_MAX
    };
    //////////////////////////////////////////////////////////////////////////
    class UIFocusable
        : public Factorable
    {
        DECLARE_FACTORABLE( UIFocusable );

    public:
        UIFocusable();
        ~UIFocusable() override;

    public:
        void setNode( const NodePtr & _node );
        const NodePtr & getNode() const;

        void setEnabled( bool _enabled );
        bool getEnabled() const;

        void setNeighbor( EUIFocusDirection _direction, const IntrusivePtr<UIFocusable> & _neighbor );
        const IntrusivePtr<UIFocusable> & getNeighbor( EUIFocusDirection _direction ) const;

        void setFocusCallback( const UIBoolValueCallbackInterfacePtr & _callback );
        const UIBoolValueCallbackInterfacePtr & getFocusCallback() const;

        void setActivateCallback( const UIActionCallbackInterfacePtr & _callback );
        const UIActionCallbackInterfacePtr & getActivateCallback() const;

    private:
        NodePtr m_node;
        IntrusivePtr<UIFocusable> m_neighbors[__UI_FOCUS_DIRECTION_MAX];
        UIBoolValueCallbackInterfacePtr m_focusCallback;
        UIActionCallbackInterfacePtr m_activateCallback;
        bool m_enabled;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UIFocusable> UIFocusablePtr;
    //////////////////////////////////////////////////////////////////////////
    class UIFocusScope
        : public Node
        , protected BaseTransformation
    {
        DECLARE_FACTORABLE( UIFocusScope );
        DECLARE_VISITABLE( Node );
        DECLARE_TRANSFORMABLE();

    public:
        UIFocusScope();
        ~UIFocusScope() override;

    public:
        UIFocusablePtr addFocusable( const NodePtr & _node );
        bool removeFocusable( const UIFocusablePtr & _focusable );
        void clearFocusables();
        uint32_t getFocusableCount() const;

        bool setFocus( const UIFocusablePtr & _focusable );
        const UIFocusablePtr & getFocus() const;
        void clearFocus();

        bool moveFocus( EUIFocusDirection _direction );
        bool focusNext();
        bool focusPrevious();
        bool activateFocus();
        bool back();

        void setBackCallback( const UIActionCallbackInterfacePtr & _callback );
        const UIActionCallbackInterfacePtr & getBackCallback() const;

    protected:
        void _dispose() override;

    private:
        bool isFocusableAvailable_( const UIFocusablePtr & _focusable ) const;
        UIFocusablePtr findSpatialFocusable_( EUIFocusDirection _direction ) const;
        int32_t findFocusableIndex_( const UIFocusablePtr & _focusable ) const;

    private:
        Vector<UIFocusablePtr> m_focusables;
        UIFocusablePtr m_focus;
        UIActionCallbackInterfacePtr m_backCallback;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<UIFocusScope> UIFocusScopePtr;
    //////////////////////////////////////////////////////////////////////////
    class UIFocusController
        : public Node
        , public InputControllerHandlerInterface
        , protected BaseTransformation
    {
        DECLARE_FACTORABLE( UIFocusController );
        DECLARE_VISITABLE( Node );
        DECLARE_TRANSFORMABLE();

    public:
        UIFocusController();
        ~UIFocusController() override;

    public:
        bool pushScope( const UIFocusScopePtr & _scope );
        bool removeScope( const UIFocusScopePtr & _scope );
        UIFocusScopePtr popScope();
        void clearScopes();
        uint32_t getScopeCount() const;
        UIFocusScopePtr getActiveScope() const;

    protected:
        bool _activate() override;
        void _deactivate() override;
        void _dispose() override;

    protected:
        void handleControllerConnectEvent( const InputControllerConnectEvent & _event ) override;
        bool handleControllerButtonEvent( const InputControllerButtonEvent & _event ) override;
        bool handleControllerAxisEvent( const InputControllerAxisEvent & _event ) override;

    private:
        void registerKeyboard_();
        void unregisterKeyboard_();
        void resetAxes_();

    private:
        Vector<UIFocusScopePtr> m_scopes;
        Vector<UniqueId> m_keyboardHandlers;
        int32_t m_axisHorizontal;
        int32_t m_axisVertical;
        bool m_inputRegistered;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<UIFocusController> UIFocusControllerPtr;
    //////////////////////////////////////////////////////////////////////////
}
