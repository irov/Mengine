#   pragma once

#	include "Interface/InputSystemInterface.h"
//#	include "Interface/ScriptSystemInterface.h"
#	include "Interface/RenderSystemInterface.h"

#   include "Math/vec2.h"

namespace Menge
{
    class Arrow;
	class Scene;
	class Scriptable;

	struct PickerTrapState
	{
		class MousePickerTrapInterface * trap;

		uint32_t id;
		bool picked;
		bool pressed;
		bool handle;
		bool dead;
	};
	
    class MousePickerTrapInterface
        : public InputSystemHandler
    {
    public:
		virtual bool pick( const mt::vec2f& _point, const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, const Viewport & _gameport, Arrow * _arrow ) = 0;

    public:
		virtual bool onHandleMouseEnter( float _x, float _y ) = 0;
        virtual void onHandleMouseLeave() = 0;
		virtual void onHandleMouseOverDestroy() = 0;

    public:
		virtual PickerTrapState * propagatePickerTrapState() const = 0;
		virtual Scriptable * propagatePickerScriptable() = 0;
    };

    typedef stdex::vector<MousePickerTrapInterface *> TVectorPickerTraps;

    class MousePickerSystemInterface
        : public InputSystemHandler
    {
    public:
        virtual void setBlock( bool _value ) = 0;
        virtual void setHandleValue( bool _value ) = 0;

    public:
        virtual void setArrow( Arrow * _arrow ) = 0;
		virtual void setScene( Scene * _scene ) = 0;
		virtual void setGameport( const Viewport & _gameport ) = 0;
		virtual void setRenderViewport( const RenderViewportInterface * _viewport ) = 0;
		virtual void setRenderCamera( const RenderCameraInterface * _camera ) = 0;
		virtual void setRenderClipplane( const RenderClipplaneInterface * _viewport ) = 0;

    public:
        virtual void update() = 0;
        virtual void clear() = 0;

    public:
        virtual bool pickTrap( const mt::vec2f& _point, TVectorPickerTraps & _traps ) = 0;

    public:
        virtual PickerTrapState * regTrap( MousePickerTrapInterface * _trap ) = 0;
        virtual void unregTrap( PickerTrapState * _id ) = 0;

		virtual void updateTrap() = 0;

    public:
        virtual uint32_t getPickerTrapCount() const = 0;

    public:
		virtual void handleMouseEnter( const InputMousePositionEvent & _event ) = 0;
		virtual void handleMouseLeave( const InputMousePositionEvent & _event ) = 0;
    };
}