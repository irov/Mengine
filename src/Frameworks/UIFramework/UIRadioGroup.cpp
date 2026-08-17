#include "UIRadioGroup.h"

#include <cstdint>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    UIRadioGroup::UIRadioGroup()
        : m_selectedIndex( UINT32_MAX )
        , m_required( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    UIRadioGroup::~UIRadioGroup()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void UIRadioGroup::setRequired( bool _required )
    {
        if( m_required == _required )
        {
            return;
        }

        m_required = _required;

        if( m_required == true && m_selectedIndex == UINT32_MAX && m_checkboxes.empty() == false )
        {
            this->setSelected_( 0, true );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool UIRadioGroup::getRequired() const
    {
        return m_required;
    }
    //////////////////////////////////////////////////////////////////////////
    void UIRadioGroup::addCheckbox( const CheckboxPtr & _checkbox )
    {
        if( _checkbox == nullptr )
        {
            return;
        }

        CheckboxDesc desc;
        desc.checkbox = _checkbox;
        desc.value = _checkbox->getValue();

        m_checkboxes.emplace_back( desc );

        this->addChild( _checkbox );

        uint32_t index = (uint32_t)m_checkboxes.size() - 1;

        if( desc.value == true )
        {
            this->setSelected_( index, true );
        }
        else if( m_required == true && m_selectedIndex == UINT32_MAX )
        {
            this->setSelected_( index, true );
        }
        else
        {
            this->refreshValues_();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    CheckboxPtr UIRadioGroup::removeCheckbox( uint32_t _index )
    {
        if( _index >= m_checkboxes.size() )
        {
            return nullptr;
        }

        CheckboxPtr checkbox = m_checkboxes[_index].checkbox;

        if( checkbox->getParent() == this )
        {
            checkbox->removeFromParent();
        }

        m_checkboxes.erase( m_checkboxes.begin() + _index );

        if( m_selectedIndex == _index )
        {
            m_selectedIndex = UINT32_MAX;

            if( m_required == true && m_checkboxes.empty() == false )
            {
                uint32_t nextIndex = _index < m_checkboxes.size() ? _index : (uint32_t)m_checkboxes.size() - 1;
                this->setSelected_( nextIndex, true );
            }
            else if( m_changeCallback != nullptr )
            {
                m_changeCallback->onUIIndexValue( UINT32_MAX );
            }
        }
        else
        {
            if( m_selectedIndex != UINT32_MAX && m_selectedIndex > _index )
            {
                --m_selectedIndex;
            }

            this->refreshValues_();
        }

        return checkbox;
    }
    //////////////////////////////////////////////////////////////////////////
    const CheckboxPtr & UIRadioGroup::getCheckbox( uint32_t _index ) const
    {
        return m_checkboxes[_index].checkbox;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t UIRadioGroup::getCheckboxCount() const
    {
        return (uint32_t)m_checkboxes.size();
    }
    //////////////////////////////////////////////////////////////////////////
    bool UIRadioGroup::select( uint32_t _index )
    {
        if( _index >= m_checkboxes.size() )
        {
            return false;
        }

        this->setSelected_( _index, true );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void UIRadioGroup::clearSelection()
    {
        if( m_required == true || m_selectedIndex == UINT32_MAX )
        {
            return;
        }

        for( CheckboxDesc & desc : m_checkboxes )
        {
            desc.checkbox->setValue( false );
            desc.value = false;
        }

        m_selectedIndex = UINT32_MAX;

        if( m_changeCallback != nullptr )
        {
            m_changeCallback->onUIIndexValue( UINT32_MAX );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t UIRadioGroup::getSelectedIndex() const
    {
        return m_selectedIndex;
    }
    //////////////////////////////////////////////////////////////////////////
    void UIRadioGroup::setChangeCallback( const UIIndexValueCallbackInterfacePtr & _callback )
    {
        m_changeCallback = _callback;
    }
    //////////////////////////////////////////////////////////////////////////
    const UIIndexValueCallbackInterfacePtr & UIRadioGroup::getChangeCallback() const
    {
        return m_changeCallback;
    }
    //////////////////////////////////////////////////////////////////////////
    void UIRadioGroup::_dispose()
    {
        m_changeCallback = nullptr;
        m_checkboxes.clear();
        m_selectedIndex = UINT32_MAX;

        Node::_dispose();
    }
    //////////////////////////////////////////////////////////////////////////
    void UIRadioGroup::update( const UpdateContext * _context )
    {
        MENGINE_UNUSED( _context );

        uint32_t changedSelection = UINT32_MAX;

        for( uint32_t index = 0; index != m_checkboxes.size(); ++index )
        {
            CheckboxDesc & desc = m_checkboxes[index];
            bool value = desc.checkbox->getValue();

            if( value == true && desc.value == false )
            {
                changedSelection = index;
            }
        }

        if( changedSelection != UINT32_MAX )
        {
            this->setSelected_( changedSelection, true );

            return;
        }

        if( m_selectedIndex != UINT32_MAX && m_checkboxes[m_selectedIndex].checkbox->getValue() == false )
        {
            if( m_required == true )
            {
                this->setSelected_( m_selectedIndex, false );
            }
            else
            {
                m_selectedIndex = UINT32_MAX;
                this->refreshValues_();

                if( m_changeCallback != nullptr )
                {
                    m_changeCallback->onUIIndexValue( UINT32_MAX );
                }
            }
        }
        else
        {
            this->refreshValues_();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void UIRadioGroup::setSelected_( uint32_t _index, bool _notify )
    {
        bool changed = m_selectedIndex != _index;

        m_selectedIndex = _index;

        for( uint32_t index = 0; index != m_checkboxes.size(); ++index )
        {
            CheckboxDesc & desc = m_checkboxes[index];
            bool value = index == m_selectedIndex;

            desc.checkbox->setValue( value );
            desc.value = value;
        }

        if( changed == true && _notify == true && m_changeCallback != nullptr )
        {
            m_changeCallback->onUIIndexValue( m_selectedIndex );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void UIRadioGroup::refreshValues_()
    {
        for( CheckboxDesc & desc : m_checkboxes )
        {
            desc.value = desc.checkbox->getValue();
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
