#include "Kernel/BasePicker.h"

#include "Kernel/Assertion.h"

#include <algorithm>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    BasePicker::BasePicker()
        : m_relationPicker( nullptr )
        , m_pickerEnable( false )
        , m_pickerPicked( false )
        , m_pickerPressed( false )
        , m_pickerHandle( false )
        , m_pickerExclusive( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    BasePicker::~BasePicker()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void BasePicker::setRelationPicker( PickerInterface * _relationPicker )
    {
        MENGINE_ASSERTION( _relationPicker != nullptr, "set nullptr relation" );
        MENGINE_ASSERTION( _relationPicker != this, "set this relation" );

        if( m_relationPicker != nullptr )
        {
            m_relationPicker->removeRelationPickerChildren_( this );
        }

        m_relationPicker = static_cast<BasePicker *>(_relationPicker);
        m_relationPicker->addRelationPickerChildrenBack_( this );
    }
    //////////////////////////////////////////////////////////////////////////
    void BasePicker::setRelationPickerFront( PickerInterface * _relationPicker )
    {
        MENGINE_ASSERTION( _relationPicker != nullptr, "set nullptr relation" );
        MENGINE_ASSERTION( _relationPicker != this, "set this relation" );

        if( m_relationPicker != nullptr )
        {
            m_relationPicker->removeRelationPickerChildren_( this );
        }

        m_relationPicker = static_cast<BasePicker *>(_relationPicker);
        m_relationPicker->addRelationPickerChildrenFront_( this );
    }
    //////////////////////////////////////////////////////////////////////////
    void BasePicker::removeRelationPicker()
    {
        if( m_relationPicker == nullptr )
        {
            return;
        }

        m_relationPicker->removeRelationPickerChildren_( this );
        m_relationPicker = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void BasePicker::moveRelationPickerFront( PickerInterface * _childPicker )
    {
        MENGINE_ASSERTION_FATAL( m_pickerChildren.empty() == false, "move child is empty" );

        BasePicker * childPicker = static_cast<BasePicker *>(_childPicker);

        BasePicker * frontPicker = m_pickerChildren.front();

        if( frontPicker == childPicker )
        {
            return;
        }

        this->removeRelationPickerChildren_( childPicker );
        this->addRelationPickerChildrenFront_( childPicker );
    }
    //////////////////////////////////////////////////////////////////////////
    void BasePicker::moveRelationPickerMiddle( PickerInterface * _afterPicker, PickerInterface * _childPicker )
    {
        MENGINE_ASSERTION_FATAL( m_pickerChildren.empty() == false, "move child is empty" );

        BasePicker * afterPicker = static_cast<BasePicker *>(_afterPicker);
        BasePicker * childPicker = static_cast<BasePicker *>(_childPicker);

        this->removeRelationPickerChildren_( childPicker );
        this->addRelationPickerChildrenAfter_( afterPicker, childPicker );
    }
    //////////////////////////////////////////////////////////////////////////
    void BasePicker::moveRelationPickerBack( PickerInterface * _childPicker )
    {
        MENGINE_ASSERTION_FATAL( m_pickerChildren.empty() == false, "move child is empty" );

        BasePicker * childPicker = static_cast<BasePicker *>(_childPicker);

        BasePicker * backPicker = m_pickerChildren.back();

        if( backPicker == childPicker )
        {
            return;
        }

        this->removeRelationPickerChildren_( childPicker );
        this->addRelationPickerChildrenBack_( childPicker );
    }
    //////////////////////////////////////////////////////////////////////////
    void BasePicker::foreachPickerChildren( const LambdaPicker & _lambda )
    {
        for( PickerInterface * child : m_pickerChildren )
        {
            _lambda( child );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void BasePicker::foreachPickerChildrenEnabled( const LambdaPicker & _lambda )
    {
        if( m_pickerEnable == false )
        {
            return;
        }

        for( BasePicker * child : m_pickerChildren )
        {
            if( child->m_pickerEnable == false )
            {
                continue;
            }

            _lambda( child );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void BasePicker::setPickerEnable( bool _enable )
    {
        m_pickerEnable = _enable;
    }
    //////////////////////////////////////////////////////////////////////////
    void BasePicker::setPickerPicked( bool _picked )
    {
        m_pickerPicked = _picked;
    }
    //////////////////////////////////////////////////////////////////////////
    void BasePicker::setPickerPressed( bool _pressed )
    {
        m_pickerPressed = _pressed;
    }
    //////////////////////////////////////////////////////////////////////////
    void BasePicker::setPickerHandle( bool _handle )
    {
        m_pickerHandle = _handle;
    }
    //////////////////////////////////////////////////////////////////////////
    void BasePicker::setPickerExclusive( bool _exclusive )
    {
        m_pickerExclusive = _exclusive;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderViewportInterfacePtr & BasePicker::getPickerViewport() const
    {
        return RenderViewportInterfacePtr::none();
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderCameraInterfacePtr & BasePicker::getPickerCamera() const
    {
        return RenderCameraInterfacePtr::none();
    }
    //////////////////////////////////////////////////////////////////////////
    void BasePicker::addRelationPickerChildrenBack_( BasePicker * _childPicker )
    {
        m_pickerChildren.push_back( _childPicker );
    }
    //////////////////////////////////////////////////////////////////////////
    void BasePicker::addRelationPickerChildrenFront_( BasePicker * _childPicker )
    {
        m_pickerChildren.insert( m_pickerChildren.begin(), _childPicker );
    }
    //////////////////////////////////////////////////////////////////////////
    void BasePicker::removeRelationPickerChildren_( BasePicker * _childPicker )
    {
        VectorBasePicker::iterator it_erase = std::find( m_pickerChildren.begin(), m_pickerChildren.end(), _childPicker );

        MENGINE_ASSERTION_FATAL( it_erase != m_pickerChildren.end(), "remove relation child is not found" );

        m_pickerChildren.erase( it_erase );
    }
    //////////////////////////////////////////////////////////////////////////
    void BasePicker::addRelationPickerChildrenAfter_( BasePicker * _afterPicker, BasePicker * _childPicker )
    {
        VectorBasePicker::iterator it_insert = std::find( m_pickerChildren.begin(), m_pickerChildren.end(), _afterPicker );

        MENGINE_ASSERTION_FATAL( it_insert != m_pickerChildren.end(), "after relation child is not found" );

        m_pickerChildren.insert( it_insert, _childPicker );
    }
}
