#include "Kernel/BasePicker.h"

#include "Interface/PickerServiceInterface.h"

#include "Kernel/Assertion.h"

#include "Config/Algorithm.h"

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
        MENGINE_ASSERTION_FATAL( _relationPicker != nullptr, "set nullptr relation" );
        MENGINE_ASSERTION_FATAL( _relationPicker != this, "set this relation" );

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
        MENGINE_ASSERTION_FATAL( _relationPicker != nullptr, "set nullptr relation" );
        MENGINE_ASSERTION_FATAL( _relationPicker != this, "set this relation" );

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
    bool BasePicker::isEmptyPickerChildren() const
    {
        bool result = m_pickerChildren.empty();

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    void BasePicker::clearPickerChildren()
    {
        for( BasePicker * child : m_pickerChildren )
        {
            child->m_relationPicker = nullptr;
        }

        m_pickerChildren.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void BasePicker::foreachPickerChildren( const LambdaPicker & _lambda )
    {
        for( BasePicker * child : m_pickerChildren )
        {
            _lambda( child );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void BasePicker::foreachPickerChildrenEnabled( const LambdaPicker & _lambda )
    {
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
    void BasePicker::updatePickers()
    {
        PICKER_SERVICE()
            ->update();
    }
    //////////////////////////////////////////////////////////////////////////
    void BasePicker::setPickerEnable( bool _enable )
    {
        if( m_pickerEnable == _enable )
        {
            return;
        }

        m_pickerEnable = _enable;

        PICKER_SERVICE()
            ->invalidateTraps();

        this->_setPickerEnable( _enable );
    }
    //////////////////////////////////////////////////////////////////////////
    void BasePicker::setPickerPicked( bool _picked )
    {
        m_pickerPicked = _picked;
    }
    //////////////////////////////////////////////////////////////////////////
    void BasePicker::_setPickerEnable( bool _enable )
    {
        MENGINE_UNUSED( _enable );

        //Empty
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
    const RenderScissorInterfacePtr & BasePicker::getPickerScissor() const
    {
        return RenderScissorInterfacePtr::none();
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderTransformationInterfacePtr & BasePicker::getPickerTransformation() const
    {
        return RenderTransformationInterfacePtr::none();
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderTargetInterfacePtr & BasePicker::getPickerTarget() const
    {
        return RenderTargetInterfacePtr::none();
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
        VectorBasePicker::iterator it_erase = Algorithm::find( m_pickerChildren.begin(), m_pickerChildren.end(), _childPicker );

        MENGINE_ASSERTION_FATAL( it_erase != m_pickerChildren.end(), "remove relation child is not found" );

        m_pickerChildren.erase( it_erase );
    }
    //////////////////////////////////////////////////////////////////////////
    void BasePicker::addRelationPickerChildrenAfter_( BasePicker * _afterPicker, BasePicker * _childPicker )
    {
        VectorBasePicker::iterator it_insert = Algorithm::find( m_pickerChildren.begin(), m_pickerChildren.end(), _afterPicker );

        MENGINE_ASSERTION_FATAL( it_insert != m_pickerChildren.end(), "after relation child is not found" );

        m_pickerChildren.insert( it_insert, _childPicker );
    }
    //////////////////////////////////////////////////////////////////////////
}
