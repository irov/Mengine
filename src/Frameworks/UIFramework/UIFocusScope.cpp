#include "UIFocusScope.h"

#include "Interface/InputServiceInterface.h"
#include "Interface/TransformationInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/GlobalInputHandlerHelper.h"

#include "Config/StdAlgorithm.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    UIFocusable::UIFocusable()
        : m_enabled( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    UIFocusable::~UIFocusable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void UIFocusable::setNode( const NodePtr & _node )
    {
        m_node = _node;
    }
    //////////////////////////////////////////////////////////////////////////
    const NodePtr & UIFocusable::getNode() const
    {
        return m_node;
    }
    //////////////////////////////////////////////////////////////////////////
    void UIFocusable::setEnabled( bool _enabled )
    {
        m_enabled = _enabled;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UIFocusable::getEnabled() const
    {
        return m_enabled;
    }
    //////////////////////////////////////////////////////////////////////////
    void UIFocusable::setNeighbor( EUIFocusDirection _direction, const UIFocusablePtr & _neighbor )
    {
        if( _direction >= __UI_FOCUS_DIRECTION_MAX )
        {
            return;
        }

        m_neighbors[_direction] = _neighbor;
    }
    //////////////////////////////////////////////////////////////////////////
    const UIFocusablePtr & UIFocusable::getNeighbor( EUIFocusDirection _direction ) const
    {
        return m_neighbors[_direction];
    }
    //////////////////////////////////////////////////////////////////////////
    void UIFocusable::setFocusCallback( const UIBoolValueCallbackInterfacePtr & _callback )
    {
        m_focusCallback = _callback;
    }
    //////////////////////////////////////////////////////////////////////////
    const UIBoolValueCallbackInterfacePtr & UIFocusable::getFocusCallback() const
    {
        return m_focusCallback;
    }
    //////////////////////////////////////////////////////////////////////////
    void UIFocusable::setActivateCallback( const UIActionCallbackInterfacePtr & _callback )
    {
        m_activateCallback = _callback;
    }
    //////////////////////////////////////////////////////////////////////////
    const UIActionCallbackInterfacePtr & UIFocusable::getActivateCallback() const
    {
        return m_activateCallback;
    }
    //////////////////////////////////////////////////////////////////////////
    UIFocusScope::UIFocusScope()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    UIFocusScope::~UIFocusScope()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    UIFocusablePtr UIFocusScope::addFocusable( const NodePtr & _node )
    {
        UIFocusablePtr focusable = Helper::makeFactorableUnique<UIFocusable>( MENGINE_DOCUMENT_FACTORABLE );
        focusable->setNode( _node );

        m_focusables.emplace_back( focusable );

        if( m_focus == nullptr && this->isFocusableAvailable_( focusable ) == true )
        {
            this->setFocus( focusable );
        }

        return focusable;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UIFocusScope::removeFocusable( const UIFocusablePtr & _focusable )
    {
        Vector<UIFocusablePtr>::iterator it_found = StdAlgorithm::find( m_focusables.begin(), m_focusables.end(), _focusable );

        if( it_found == m_focusables.end() )
        {
            return false;
        }

        bool focused = m_focus == _focusable;

        if( focused == true )
        {
            this->clearFocus();
        }

        m_focusables.erase( it_found );

        if( focused == true )
        {
            this->focusNext();
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void UIFocusScope::clearFocusables()
    {
        this->clearFocus();
        m_focusables.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t UIFocusScope::getFocusableCount() const
    {
        return (uint32_t)m_focusables.size();
    }
    //////////////////////////////////////////////////////////////////////////
    bool UIFocusScope::setFocus( const UIFocusablePtr & _focusable )
    {
        if( this->isFocusableAvailable_( _focusable ) == false )
        {
            return false;
        }

        if( this->findFocusableIndex_( _focusable ) < 0 )
        {
            return false;
        }

        if( m_focus == _focusable )
        {
            return true;
        }

        if( m_focus != nullptr )
        {
            const UIBoolValueCallbackInterfacePtr & oldCallback = m_focus->getFocusCallback();

            if( oldCallback != nullptr )
            {
                oldCallback->onUIBoolValue( false );
            }
        }

        m_focus = _focusable;

        const UIBoolValueCallbackInterfacePtr & callback = m_focus->getFocusCallback();

        if( callback != nullptr )
        {
            callback->onUIBoolValue( true );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const UIFocusablePtr & UIFocusScope::getFocus() const
    {
        return m_focus;
    }
    //////////////////////////////////////////////////////////////////////////
    void UIFocusScope::clearFocus()
    {
        if( m_focus == nullptr )
        {
            return;
        }

        const UIBoolValueCallbackInterfacePtr & callback = m_focus->getFocusCallback();

        if( callback != nullptr )
        {
            callback->onUIBoolValue( false );
        }

        m_focus = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UIFocusScope::moveFocus( EUIFocusDirection _direction )
    {
        if( _direction >= __UI_FOCUS_DIRECTION_MAX )
        {
            return false;
        }

        if( m_focus == nullptr )
        {
            return this->focusNext();
        }

        const UIFocusablePtr & explicitNeighbor = m_focus->getNeighbor( _direction );

        if( this->isFocusableAvailable_( explicitNeighbor ) == true )
        {
            return this->setFocus( explicitNeighbor );
        }

        UIFocusablePtr spatialFocusable = this->findSpatialFocusable_( _direction );

        if( spatialFocusable != nullptr )
        {
            return this->setFocus( spatialFocusable );
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UIFocusScope::focusNext()
    {
        if( m_focusables.empty() == true )
        {
            return false;
        }

        int32_t currentIndex = this->findFocusableIndex_( m_focus );
        uint32_t startIndex = currentIndex < 0 ? 0 : (uint32_t)currentIndex + 1;

        for( uint32_t offset = 0; offset != m_focusables.size(); ++offset )
        {
            uint32_t index = (startIndex + offset) % (uint32_t)m_focusables.size();

            if( this->isFocusableAvailable_( m_focusables[index] ) == true )
            {
                return this->setFocus( m_focusables[index] );
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UIFocusScope::focusPrevious()
    {
        if( m_focusables.empty() == true )
        {
            return false;
        }

        int32_t currentIndex = this->findFocusableIndex_( m_focus );
        uint32_t startIndex = currentIndex < 0 ? 0 : (uint32_t)currentIndex;

        for( uint32_t offset = 1; offset <= m_focusables.size(); ++offset )
        {
            uint32_t index = (startIndex + (uint32_t)m_focusables.size() - offset) % (uint32_t)m_focusables.size();

            if( this->isFocusableAvailable_( m_focusables[index] ) == true )
            {
                return this->setFocus( m_focusables[index] );
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UIFocusScope::activateFocus()
    {
        if( this->isFocusableAvailable_( m_focus ) == false )
        {
            return false;
        }

        const UIActionCallbackInterfacePtr & callback = m_focus->getActivateCallback();

        if( callback == nullptr )
        {
            return false;
        }

        callback->onUIAction();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UIFocusScope::back()
    {
        if( m_backCallback == nullptr )
        {
            return false;
        }

        m_backCallback->onUIAction();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void UIFocusScope::setBackCallback( const UIActionCallbackInterfacePtr & _callback )
    {
        m_backCallback = _callback;
    }
    //////////////////////////////////////////////////////////////////////////
    const UIActionCallbackInterfacePtr & UIFocusScope::getBackCallback() const
    {
        return m_backCallback;
    }
    //////////////////////////////////////////////////////////////////////////
    void UIFocusScope::_dispose()
    {
        this->clearFocusables();
        m_backCallback = nullptr;

        Node::_dispose();
    }
    //////////////////////////////////////////////////////////////////////////
    bool UIFocusScope::isFocusableAvailable_( const UIFocusablePtr & _focusable ) const
    {
        if( _focusable == nullptr || _focusable->getEnabled() == false )
        {
            return false;
        }

        const NodePtr & node = _focusable->getNode();

        return node != nullptr && node->isEnable() == true;
    }
    //////////////////////////////////////////////////////////////////////////
    UIFocusablePtr UIFocusScope::findSpatialFocusable_( EUIFocusDirection _direction ) const
    {
        if( m_focus == nullptr )
        {
            return nullptr;
        }

        const NodePtr & currentNode = m_focus->getNode();
        TransformationInterface * currentTransformation = currentNode->getTransformation();

        if( currentTransformation == nullptr )
        {
            return nullptr;
        }

        mt::vec3f currentPosition3 = currentTransformation->getWorldPosition();
        mt::vec2f currentPosition( currentPosition3.x, currentPosition3.y );

        mt::vec2f direction;

        switch( _direction )
        {
        case UIFD_LEFT:
            direction = mt::vec2f( -1.f, 0.f );
            break;
        case UIFD_RIGHT:
            direction = mt::vec2f( 1.f, 0.f );
            break;
        case UIFD_UP:
            direction = mt::vec2f( 0.f, -1.f );
            break;
        case UIFD_DOWN:
            direction = mt::vec2f( 0.f, 1.f );
            break;
        default:
            return nullptr;
        }

        UIFocusablePtr bestFocusable;
        float bestScore = 0.f;

        for( const UIFocusablePtr & focusable : m_focusables )
        {
            if( focusable == m_focus || this->isFocusableAvailable_( focusable ) == false )
            {
                continue;
            }

            TransformationInterface * transformation = focusable->getNode()->getTransformation();

            if( transformation == nullptr )
            {
                continue;
            }

            mt::vec3f position3 = transformation->getWorldPosition();
            mt::vec2f delta( position3.x - currentPosition.x, position3.y - currentPosition.y );
            float primary = delta.x * direction.x + delta.y * direction.y;

            if( primary <= 0.f )
            {
                continue;
            }

            float orthogonal = delta.x * direction.y - delta.y * direction.x;

            if( orthogonal < 0.f )
            {
                orthogonal = -orthogonal;
            }

            float score = primary + orthogonal * 2.f;

            if( bestFocusable == nullptr || score < bestScore )
            {
                bestFocusable = focusable;
                bestScore = score;
            }
        }

        return bestFocusable;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t UIFocusScope::findFocusableIndex_( const UIFocusablePtr & _focusable ) const
    {
        Vector<UIFocusablePtr>::const_iterator it_found = StdAlgorithm::find( m_focusables.begin(), m_focusables.end(), _focusable );

        if( it_found == m_focusables.end() )
        {
            return -1;
        }

        return (int32_t)(it_found - m_focusables.begin());
    }
    //////////////////////////////////////////////////////////////////////////
    UIFocusController::UIFocusController()
        : m_axisHorizontal( 0 )
        , m_axisVertical( 0 )
        , m_inputRegistered( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    UIFocusController::~UIFocusController()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool UIFocusController::pushScope( const UIFocusScopePtr & _scope )
    {
        if( _scope == nullptr || StdAlgorithm::find( m_scopes.begin(), m_scopes.end(), _scope ) != m_scopes.end() )
        {
            return false;
        }

        m_scopes.emplace_back( _scope );
        this->resetAxes_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UIFocusController::removeScope( const UIFocusScopePtr & _scope )
    {
        Vector<UIFocusScopePtr>::iterator it_found = StdAlgorithm::find( m_scopes.begin(), m_scopes.end(), _scope );

        if( it_found == m_scopes.end() )
        {
            return false;
        }

        m_scopes.erase( it_found );
        this->resetAxes_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    UIFocusScopePtr UIFocusController::popScope()
    {
        if( m_scopes.empty() == true )
        {
            return nullptr;
        }

        UIFocusScopePtr scope = m_scopes.back();
        m_scopes.pop_back();
        this->resetAxes_();

        return scope;
    }
    //////////////////////////////////////////////////////////////////////////
    void UIFocusController::clearScopes()
    {
        m_scopes.clear();
        this->resetAxes_();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t UIFocusController::getScopeCount() const
    {
        return (uint32_t)m_scopes.size();
    }
    //////////////////////////////////////////////////////////////////////////
    UIFocusScopePtr UIFocusController::getActiveScope() const
    {
        for( Vector<UIFocusScopePtr>::const_reverse_iterator
            it = m_scopes.rbegin(),
            it_end = m_scopes.rend();
            it != it_end;
            ++it )
        {
            const UIFocusScopePtr & scope = *it;

            if( scope != nullptr && scope->isActivate() == true && scope->isEnable() == true )
            {
                return scope;
            }
        }

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UIFocusController::_activate()
    {
        INPUT_SERVICE()
            ->addControllerHandler( InputControllerHandlerInterfacePtr::from( this ), MENGINE_DOCUMENT_FACTORABLE );

        m_inputRegistered = true;
        this->registerKeyboard_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void UIFocusController::_deactivate()
    {
        this->unregisterKeyboard_();

        if( m_inputRegistered == true )
        {
            INPUT_SERVICE()
                ->removeControllerHandler( InputControllerHandlerInterfacePtr::from( this ) );

            m_inputRegistered = false;
        }

        this->resetAxes_();
    }
    //////////////////////////////////////////////////////////////////////////
    void UIFocusController::_dispose()
    {
        m_scopes.clear();

        Node::_dispose();
    }
    //////////////////////////////////////////////////////////////////////////
    void UIFocusController::handleControllerConnectEvent( const InputControllerConnectEvent & _event )
    {
        MENGINE_UNUSED( _event );

        this->resetAxes_();
    }
    //////////////////////////////////////////////////////////////////////////
    bool UIFocusController::handleControllerButtonEvent( const InputControllerButtonEvent & _event )
    {
        if( _event.isDown == false )
        {
            return false;
        }

        UIFocusScopePtr scope = this->getActiveScope();

        if( scope == nullptr )
        {
            return false;
        }

        switch( _event.button )
        {
        case CB_DPAD_LEFT:
            return scope->moveFocus( UIFD_LEFT );
        case CB_DPAD_RIGHT:
            return scope->moveFocus( UIFD_RIGHT );
        case CB_DPAD_UP:
            return scope->moveFocus( UIFD_UP );
        case CB_DPAD_DOWN:
            return scope->moveFocus( UIFD_DOWN );
        case CB_A:
            return scope->activateFocus();
        case CB_B:
            return scope->back();
        default:
            return false;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool UIFocusController::handleControllerAxisEvent( const InputControllerAxisEvent & _event )
    {
        UIFocusScopePtr scope = this->getActiveScope();

        if( scope == nullptr )
        {
            this->resetAxes_();

            return false;
        }

        if( _event.axis == CA_LEFT_X )
        {
            int32_t direction = _event.value < -0.5f ? -1 : _event.value > 0.5f ? 1 : 0;

            if( direction != 0 && m_axisHorizontal == 0 )
            {
                m_axisHorizontal = direction;

                return scope->moveFocus( direction < 0 ? UIFD_LEFT : UIFD_RIGHT );
            }

            m_axisHorizontal = direction;
        }
        else if( _event.axis == CA_LEFT_Y )
        {
            int32_t direction = _event.value < -0.5f ? -1 : _event.value > 0.5f ? 1 : 0;

            if( direction != 0 && m_axisVertical == 0 )
            {
                m_axisVertical = direction;

                return scope->moveFocus( direction < 0 ? UIFD_DOWN : UIFD_UP );
            }

            m_axisVertical = direction;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void UIFocusController::registerKeyboard_()
    {
        m_keyboardHandlers.emplace_back( Helper::addGlobalKeyOnceHandler( KC_LEFT, true, [this]( const InputKeyEvent & )
        {
            UIFocusScopePtr scope = this->getActiveScope();

            if( scope != nullptr )
            {
                scope->moveFocus( UIFD_LEFT );
            }
        }, MENGINE_DOCUMENT_FACTORABLE ) );

        m_keyboardHandlers.emplace_back( Helper::addGlobalKeyOnceHandler( KC_RIGHT, true, [this]( const InputKeyEvent & )
        {
            UIFocusScopePtr scope = this->getActiveScope();

            if( scope != nullptr )
            {
                scope->moveFocus( UIFD_RIGHT );
            }
        }, MENGINE_DOCUMENT_FACTORABLE ) );

        m_keyboardHandlers.emplace_back( Helper::addGlobalKeyOnceHandler( KC_UP, true, [this]( const InputKeyEvent & )
        {
            UIFocusScopePtr scope = this->getActiveScope();

            if( scope != nullptr )
            {
                scope->moveFocus( UIFD_UP );
            }
        }, MENGINE_DOCUMENT_FACTORABLE ) );

        m_keyboardHandlers.emplace_back( Helper::addGlobalKeyOnceHandler( KC_DOWN, true, [this]( const InputKeyEvent & )
        {
            UIFocusScopePtr scope = this->getActiveScope();

            if( scope != nullptr )
            {
                scope->moveFocus( UIFD_DOWN );
            }
        }, MENGINE_DOCUMENT_FACTORABLE ) );

        m_keyboardHandlers.emplace_back( Helper::addGlobalKeyOnceHandler( KC_TAB, true, [this]( const InputKeyEvent & _event )
        {
            UIFocusScopePtr scope = this->getActiveScope();

            if( scope != nullptr )
            {
                if( _event.special.isShift == true )
                {
                    scope->focusPrevious();
                }
                else
                {
                    scope->focusNext();
                }
            }
        }, MENGINE_DOCUMENT_FACTORABLE ) );

        m_keyboardHandlers.emplace_back( Helper::addGlobalKeyOnceHandler( KC_RETURN, true, [this]( const InputKeyEvent & )
        {
            UIFocusScopePtr scope = this->getActiveScope();

            if( scope != nullptr )
            {
                scope->activateFocus();
            }
        }, MENGINE_DOCUMENT_FACTORABLE ) );

        m_keyboardHandlers.emplace_back( Helper::addGlobalKeyOnceHandler( KC_SPACE, true, [this]( const InputKeyEvent & )
        {
            UIFocusScopePtr scope = this->getActiveScope();

            if( scope != nullptr )
            {
                scope->activateFocus();
            }
        }, MENGINE_DOCUMENT_FACTORABLE ) );

        m_keyboardHandlers.emplace_back( Helper::addGlobalKeyOnceHandler( KC_ESCAPE, true, [this]( const InputKeyEvent & )
        {
            UIFocusScopePtr scope = this->getActiveScope();

            if( scope != nullptr )
            {
                scope->back();
            }
        }, MENGINE_DOCUMENT_FACTORABLE ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void UIFocusController::unregisterKeyboard_()
    {
        for( UniqueId handlerId : m_keyboardHandlers )
        {
            Helper::removeGlobalHandler( handlerId );
        }

        m_keyboardHandlers.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void UIFocusController::resetAxes_()
    {
        m_axisHorizontal = 0;
        m_axisVertical = 0;
    }
    //////////////////////////////////////////////////////////////////////////
}
