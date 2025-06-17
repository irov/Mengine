#pragma once

#include "Interface/DocumentInterface.h"
#include "Interface/PickerInputHandlerInterface.h"

#include "Kernel/HotSpotEventReceiverInterface.h"

#include "Kernel/Polygon.h"
#include "Kernel/BaseEventation.h"
#include "Kernel/BasePicker.h"
#include "Kernel/BaseTransformation.h"
#include "Kernel/Node.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class HotSpot
        : public Node
        , public PickerInputHandlerInterface
        , protected BaseEventation
        , protected BasePicker
        , protected BaseTransformation
    {
        DECLARE_VISITABLE( Node );
        DECLARE_FACTORABLE( HotSpot );
        DECLARE_EVENTABLE();
        DECLARE_PICKERABLE();
        DECLARE_TRANSFORMABLE();

    public:
        HotSpot();
        ~HotSpot() override;

    public:
        void setOutward( bool _value );
        bool getOutward() const;

    public:
        void setGlobal( bool _value );
        bool getGlobal() const;

    public:
        void setExclusive( bool _value );
        bool getExclusive() const;

    public:
        void setDefaultHandle( bool _handle );
        bool getDefaultHandle() const;

    public:
        bool isMousePickerOver() const;

    public:
        void addPickerInputHandler( const PickerInputHandlerInterfacePtr & _inputHandler, const DocumentInterfacePtr & _doc );
        void removePickerInputHandler( const PickerInputHandlerInterfacePtr & _inputHandler );

    public:
        virtual bool testPoint( const RenderContext * _context, const mt::vec2f & _point ) const = 0;
        virtual bool testRadius( const RenderContext * _context, const mt::vec2f & _point, float _radius ) const = 0;
        virtual bool testPolygon( const RenderContext * _context, const mt::vec2f & _point, const Polygon & _polygon ) const = 0;

    public:
        virtual bool testBounds( const RenderContext * _context, float _left, float _right, float _up, float _bottom ) const = 0;

    protected:
        bool _activate() override;
        bool _afterActivate() override;
        void _deactivate() override;

    protected:
        void _dispose() override;

    protected:
        bool pick( const mt::vec2f & _point, const RenderContext * _context ) const override;

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    public:
        Scriptable * getPickerScriptable() override;
#endif

    public:
        Eventable * getPickerEventable() override;
        PickerInputHandlerInterface * getPickerInputHandler() override;

    public:
        bool handleKeyEvent( const RenderContext * _context, const InputKeyEvent & _event ) override;
        bool handleTextEvent( const RenderContext * _context, const InputTextEvent & _event ) override;

    public:
        bool handleMouseButtonEvent( const RenderContext * _context, const InputMouseButtonEvent & _event ) override;
        bool handleMouseButtonEventBegin( const RenderContext * _context, const InputMouseButtonEvent & _event ) override;
        bool handleMouseButtonEventEnd( const RenderContext * _context, const InputMouseButtonEvent & _event ) override;
        bool handleMouseMove( const RenderContext * _context, const InputMouseMoveEvent & _event ) override;
        bool handleMouseWheel( const RenderContext * _context, const InputMouseWheelEvent & _event ) override;

    protected:
        bool handleMouseEnter( const RenderContext * _context, const InputMouseEnterEvent & _event ) override;
        void handleMouseLeave( const RenderContext * _context, const InputMouseLeaveEvent & _event ) override;

    protected:
        void _setPickerEnable( bool _enable ) override;

    protected:
        void _invalidateWorldMatrix() const override;

    protected:
        struct PickerInputHandlerDesc
        {
            PickerInputHandlerInterfacePtr handler;

#if defined(MENGINE_DOCUMENT_ENABLE)
            DocumentInterfacePtr doc;
#endif
        };

        typedef Vector<PickerInputHandlerDesc> VectorPickerInputHandlers;
        VectorPickerInputHandlers m_pickerInputHandlers;

        bool m_outward;
        bool m_global;

        bool m_defaultHandle;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<HotSpot> HotSpotPtr;
    //////////////////////////////////////////////////////////////////////////
}
