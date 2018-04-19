#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ScheduleManagerInterface.h"
#include "Interface/MousePickerSystemInterface.h"
#include "Interface/GlobalHandleSystemInterface.h"
#include "Interface/NodeInterface.h"

namespace Mengine
{
    class Node;
    class Scene;
    class Arrow;
	class Affectorable;
    class RenderCameraInterface;
    class RenderViewportInterface;
    class RenderClipplaneInterface;
	//////////////////////////////////////////////////////////////////////////
	class SceneChangeCallbackInterface
		: public FactorablePtr
	{
	public:
		virtual void onSceneChange( Scene * _scene, bool _enable, bool _remove ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<SceneChangeCallbackInterface> SceneChangeCallbackInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
    class PlayerServiceInterface
        : public ServiceInterface
		, public InputHandlerInterface
    {
        SERVICE_DECLARE("PlayerService")

    public:
		virtual bool setCurrentScene( Scene * _scene, bool _destroyOld, const SceneChangeCallbackInterfacePtr & _cb ) = 0;
		virtual bool restartCurrentScene( const SceneChangeCallbackInterfacePtr & _cb ) = 0;
		virtual bool removeCurrentScene( const SceneChangeCallbackInterfacePtr & _cb ) = 0;
		virtual void destroyCurrentScene() = 0;
        
		virtual Scene * getCurrentScene() = 0;

    public:
		virtual bool createGlobalScene() = 0;
		virtual void removeGlobalScene() = 0;
		virtual Scene * getGlobalScene() = 0;

	public:
		virtual void onFocus( bool _focus ) = 0;

		virtual void onAppMouseLeave( const InputMousePositionEvent & _event ) = 0;
		virtual void onAppMouseEnter( const InputMousePositionEvent & _event ) = 0;
		virtual void onAppMousePosition( const InputMousePositionEvent & _event ) = 0;

		virtual void onFullscreen( const Resolution & _resolution, bool _fullscreen ) = 0;
		virtual void onFixedContentResolution( const Resolution & _resolution, bool _fixed ) = 0;
		virtual void onFixedDisplayResolution( const Resolution & _resolution, bool _fixed ) = 0;

	public:
		virtual void tick( float _time, float _timing ) = 0;
		virtual bool update() = 0;
		virtual void render() = 0;

	public:
		virtual void initializeRenderResources() = 0;
		virtual void finalizeRenderResources() = 0;


    public:
        virtual void setArrow( Arrow * _arrow ) = 0;
        virtual Arrow * getArrow() const = 0;

	public:
		virtual void calcGlobalMouseWorldPosition( const mt::vec2f & _screenPoint, mt::vec2f & _worldPoint ) = 0;
		virtual void calcGlobalMouseWorldDelta( const mt::vec2f & _screenPoint, const mt::vec2f & _screenDeltha, mt::vec2f & _worldDeltha ) = 0;

	public:
		virtual ScheduleManagerInterfacePtr createSchedulerManager() = 0;
		virtual bool destroySchedulerManager( const ScheduleManagerInterfacePtr & _scheduler ) = 0;

    public:
        virtual void setRenderCamera( RenderCameraInterface * _camera) = 0;
        virtual const RenderCameraInterface * getRenderCamera() const = 0;

	public:
		virtual void setRenderViewport( RenderViewportInterface * _renderViewport ) = 0;
		virtual const RenderViewportInterface * getRenderViewport() const = 0;

	public:
		virtual void setRenderClipplane( RenderClipplaneInterface * _clipplane ) = 0;
		virtual const RenderClipplaneInterface * getRenderClipplane() const = 0;
        
    public:
        virtual MousePickerSystemInterface * getMousePickerSystem() const = 0;
        virtual GlobalHandleSystemInterface * getGlobalHandleSystem() const = 0;

    public:
        virtual const ScheduleManagerInterfacePtr & getScheduleManager() const = 0;
        virtual const ScheduleManagerInterfacePtr & getScheduleManagerGlobal() const = 0;

	public:
		virtual Affectorable * getAffectorable() const = 0;
		virtual Affectorable * getAffectorableGlobal() const = 0;
		
    public:
        virtual void toggleDebugText() = 0;
    };

#   define PLAYER_SERVICE()\
    ((Mengine::PlayerServiceInterface*)SERVICE_GET(Mengine::PlayerServiceInterface))
}