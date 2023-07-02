#pragma once

#include "Interface/DocumentInterface.h"

#include "HotSpotEventReceiverInterface.h"

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
        , public InputHandlerInterface
        , protected BaseEventation
        , protected BasePicker
        , protected BaseTransformation
    {
        DECLARE_VISITABLE( Node );
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
        void addInputHandler( const InputHandlerInterfacePtr & _inputHandler, const DocumentInterfacePtr & _doc );
        void removeInputHandler( const InputHandlerInterfacePtr & _inputHandler );

    public:
        virtual bool testPoint( const RenderContext * _context, const Resolution & _contentResolution, const mt::vec2f & _point ) const = 0;
        virtual bool testRadius( const RenderContext * _context, const Resolution & _contentResolution, const mt::vec2f & _point, float _radius ) const = 0;
        virtual bool testPolygon( const RenderContext * _context, const Resolution & _contentResolution, const mt::vec2f & _point, const Polygon & _polygon ) const = 0;

    protected:
        bool _activate() override;
        bool _afterActivate() override;
        void _deactivate() override;

    protected:
        void _dispose() override;

    protected:
        void _freeze( bool _value ) override;

    protected:
        bool pick( const mt::vec2f & _point, const RenderContext * _context, const Resolution & _contentResolution, const ArrowPtr & _arrow ) const override;

#if defined(MENGINE_USE_SCRIPT_SERVICE)
    public:
        Scriptable * getPickerScriptable() override;
#endif

    public:
        Eventable * getPickerEventable() override;
        InputHandlerInterface * getPickerInputHandler() override;

    public:
        bool handleKeyEvent( const InputKeyEvent & _event ) override;
        bool handleTextEvent( const InputTextEvent & _event ) override;

    public:
        bool handleMouseButtonEvent( const InputMouseButtonEvent & _event ) override;
        bool handleMouseButtonEventBegin( const InputMouseButtonEvent & _event ) override;
        bool handleMouseButtonEventEnd( const InputMouseButtonEvent & _event ) override;
        bool handleMouseMove( const InputMouseMoveEvent & _event ) override;
        bool handleMouseWheel( const InputMouseWheelEvent & _event ) override;

    protected:
        bool handleMouseEnter( const InputMouseEnterEvent & _event ) override;
        void handleMouseLeave( const InputMouseLeaveEvent & _event ) override;

    protected:
        void activatePicker_();
        void deactivatePicker_();

    protected:
        void _setPickerEnable( bool _enable ) override;

    protected:
        void _invalidateWorldMatrix() const override;

    protected:
        struct InputHandlerDesc
        {
            InputHandlerInterfacePtr handler;

#if defined(MENGINE_DOCUMENT_ENABLE)
            DocumentInterfacePtr doc;
#endif
        };

        typedef Vector<InputHandlerDesc> VectorInputHandlers;
        VectorInputHandlers m_inputHandlers;

        bool m_outward;
        bool m_global;
        bool m_exclusive;

        bool m_defaultHandle;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<HotSpot> HotSpotPtr;
    //////////////////////////////////////////////////////////////////////////
}
