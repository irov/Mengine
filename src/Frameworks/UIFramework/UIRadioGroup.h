#pragma once

#include "UIControlCallbackInterface.h"
#include "Checkbox.h"

#include "Kernel/Node.h"
#include "Kernel/BaseTransformation.h"
#include "Kernel/BaseUpdation.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class UIRadioGroup
        : public Node
        , protected BaseTransformation
        , protected BaseUpdation
    {
        DECLARE_FACTORABLE( UIRadioGroup );
        DECLARE_VISITABLE( Node );
        DECLARE_TRANSFORMABLE();
        DECLARE_UPDATABLE();

    public:
        UIRadioGroup();
        ~UIRadioGroup() override;

    public:
        void setRequired( bool _required );
        bool getRequired() const;

        void addCheckbox( const CheckboxPtr & _checkbox );
        CheckboxPtr removeCheckbox( uint32_t _index );
        const CheckboxPtr & getCheckbox( uint32_t _index ) const;
        uint32_t getCheckboxCount() const;

        bool select( uint32_t _index );
        void clearSelection();
        uint32_t getSelectedIndex() const;

        void setChangeCallback( const UIIndexValueCallbackInterfacePtr & _callback );
        const UIIndexValueCallbackInterfacePtr & getChangeCallback() const;

    protected:
        void _dispose() override;
        void update( const UpdateContext * _context ) override;

    private:
        void setSelected_( uint32_t _index, bool _notify );
        void refreshValues_();

    private:
        struct CheckboxDesc
        {
            CheckboxPtr checkbox;
            bool value;
        };

        typedef Vector<CheckboxDesc> VectorCheckboxes;

        VectorCheckboxes m_checkboxes;
        UIIndexValueCallbackInterfacePtr m_changeCallback;
        uint32_t m_selectedIndex;
        bool m_required;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<UIRadioGroup> UIRadioGroupPtr;
    //////////////////////////////////////////////////////////////////////////
}
