#pragma once

#include "Interface/PickerInterface.h"

#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class BasePicker
        : public PickerInterface
    {
    public:
        BasePicker();
        ~BasePicker() override;

    public:
        void setRelationPicker( PickerInterface * _relationPicker ) override;
        void setRelationPickerFront( PickerInterface * _relationPicker ) override;
        void removeRelationPicker() override;
        MENGINE_INLINE BasePicker * getRelationPicker() const override;

    public:
        bool emptyPickerChildren() const override;
        void clearPickerChildren() override;

    public:
        void foreachPickerChildren( const LambdaPicker & _lambda ) override;
        void foreachPickerChildrenEnabled( const LambdaPicker & _lambda ) override;

    public:
        void updatePickers() override;

    public:
        void setPickerEnable( bool _enable ) override;
        MENGINE_INLINE bool isPickerEnable() const override;

    public:
        virtual void _setPickerEnable( bool _enable );

    public:
        MENGINE_INLINE bool isPickerDummy() const override;

    public:
        void setPickerPicked( bool _picked ) override;
        MENGINE_INLINE bool isPickerPicked() const override;
        void setPickerPressed( bool _pressed ) override;
        MENGINE_INLINE bool isPickerPressed() const override;
        void setPickerHandle( bool _handle ) override;
        MENGINE_INLINE bool isPickerHandle() const override;
        void setPickerExclusive( bool _exclusive ) override;
        MENGINE_INLINE bool isPickerExclusive() const override;

    protected:
        const RenderViewportInterfacePtr & getPickerViewport() const override;
        const RenderCameraInterfacePtr & getPickerCamera() const override;
        const RenderScissorInterfacePtr & getPickerScissor() const override;
        const RenderTransformationInterfacePtr & getPickerTransformation() const override;
        const RenderTargetInterfacePtr & getPickerTarget() const override;

    protected:
        void addRelationPickerChildrenBack_( BasePicker * _childPicker );
        void addRelationPickerChildrenFront_( BasePicker * _childPicker );
        void removeRelationPickerChildren_( BasePicker * _childPicker );
        void addRelationPickerChildrenAfter_( BasePicker * _afterPicker, BasePicker * _childPicker );

    protected:
        BasePicker * m_relationPicker;

        typedef Vector<BasePicker *> VectorBasePicker;
        VectorBasePicker m_pickerChildren;

        bool m_pickerEnable;

        bool m_pickerPicked;
        bool m_pickerPressed;
        bool m_pickerHandle;
        bool m_pickerExclusive;
    };
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE BasePicker * BasePicker::getRelationPicker() const
    {
        return m_relationPicker;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool BasePicker::isPickerEnable() const
    {
        return m_pickerEnable;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool BasePicker::isPickerDummy() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool BasePicker::isPickerPicked() const
    {
        return m_pickerPicked;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool BasePicker::isPickerPressed() const
    {
        return m_pickerPressed;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool BasePicker::isPickerHandle() const
    {
        return m_pickerHandle;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool BasePicker::isPickerExclusive() const
    {
        return m_pickerExclusive;
    }
    //////////////////////////////////////////////////////////////////////////
}