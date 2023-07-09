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
        void setRelationPicker( PickerInterface * _relationPicker ) override final;
        void setRelationPickerFront( PickerInterface * _relationPicker ) override final;
        void removeRelationPicker() override final;
        MENGINE_INLINE BasePicker * getRelationPicker() const override final;

    public:
        bool isEmptyPickerChildren() const override final;
        void clearPickerChildren() override final;

    public:
        void foreachPickerChildren( const LambdaPicker & _lambda ) override final;
        void foreachPickerChildrenEnabled( const LambdaPicker & _lambda ) override final;

    public:
        void updatePickers() override final;

    public:
        void setPickerEnable( bool _enable ) override final;
        MENGINE_INLINE bool isPickerEnable() const override final;

    public:
        virtual void _setPickerEnable( bool _enable );

    public:
        MENGINE_INLINE bool isPickerDummy() const override;

    public:
        void setPickerPicked( bool _picked ) override final;
        MENGINE_INLINE bool isPickerPicked() const override final;
        void setPickerPressed( bool _pressed ) override final;
        MENGINE_INLINE bool isPickerPressed() const override final;
        void setPickerHandle( bool _handle ) override final;
        MENGINE_INLINE bool isPickerHandle() const override final;
        void setPickerExclusive( bool _exclusive ) override final;
        MENGINE_INLINE bool isPickerExclusive() const override final;

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