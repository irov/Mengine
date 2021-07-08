#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/InputHandlerInterface.h"

#include "Kernel/Scene.h"
#include "Kernel/Vector.h"

#include "math/vec2.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<PickerInterfacePtr> VectorPickers;
    //////////////////////////////////////////////////////////////////////////
    class PickerServiceInterface
        : public ServiceInterface
        , public InputHandlerInterface
    {
        SERVICE_DECLARE( "PickerService" );

    public:
        virtual void setBlock( bool _value ) = 0;
        virtual void setHandleValue( bool _value ) = 0;

    public:
        virtual void setArrow( const ArrowPtr & _arrow ) = 0;
        virtual void setScene( const ScenePtr & _scene ) = 0;

    public:
        virtual void setRenderViewport( const RenderViewportInterfacePtr & _viewport ) = 0;
        virtual void setRenderTransformation( const RenderTransformationInterfacePtr & _transformation ) = 0;
        virtual void setRenderCamera( const RenderCameraInterfacePtr & _camera ) = 0;
        virtual void setRenderScissor( const RenderScissorInterfacePtr & _scissor ) = 0;
        virtual void setRenderTarget( const RenderTargetInterfacePtr & _target ) = 0;

    public:
        virtual void update() = 0;

    public:
        virtual bool pickTraps( const mt::vec2f & _point, ETouchCode _touchId, float _pressure, const InputSpecialData & _special, VectorPickers * const _pickers ) const = 0;
        virtual bool getTraps( const mt::vec2f & _point, VectorPickers * const _pickers ) const = 0;

    public:
        virtual void invalidateTraps() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define PICKER_SERVICE()\
    ((Mengine::PickerServiceInterface *)SERVICE_GET(Mengine::PickerServiceInterface))
//////////////////////////////////////////////////////////////////////////