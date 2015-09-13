#   pragma once

#   include "Interface/ServiceInterface.h"
#   include "Interface/ScheduleManagerInterface.h"
#   include "Interface/MousePickerSystemInterface.h"
#   include "Interface/GlobalHandleSystemInterface.h"
#   include "Interface/RenderSystemInterface.h"
#   include "Interface/NodeInterface.h"
#   include "Interface/ScriptSystemInterface.h"

namespace Menge
{
    class Join;
    class Node;
    class Scene;
    class Arrow;
	class Affectorable;

    typedef stdex::vector<Node *> TVectorNode;
    
    class PlayerServiceInterface
        : public ServiceInterface
		, public InputSystemHandler
    {
        SERVICE_DECLARE("PlayerService")

	public:
		virtual bool initialize() = 0;
		virtual void finalize() = 0;

    public:
		virtual bool setCurrentScene( Scene * _scene, bool _destroyOld, const pybind::object & _cb ) = 0;
		virtual bool restartCurrentScene( const pybind::object & _cb ) = 0;
		virtual bool removeCurrentScene( const pybind::object & _cb ) = 0;
        
		virtual Scene * getCurrentScene() = 0;

		virtual bool isChangedScene() const = 0;

	public:
		virtual void onFocus( bool _focus ) = 0;

		virtual void onAppMouseLeave( const InputMousePositionEvent & _event ) = 0;
		virtual void onAppMouseEnter( const InputMousePositionEvent & _event ) = 0;
		virtual void onAppMousePosition( const InputMousePositionEvent & _event ) = 0;

		virtual void onFullscreen( const Resolution & _resolution, bool _fullscreen ) = 0;
		virtual void onFixedContentResolution( const Resolution & _resolution, bool _fixed ) = 0;

	public:
        virtual float getTime() const = 0;

	public:
		virtual void tick( float _timing ) = 0;
		virtual bool update() = 0;
		virtual void render() = 0;

	public:
		virtual void updateChangeScene() = 0;

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
		virtual ScheduleManagerInterface * createSchedulerManager() = 0;
		virtual bool destroySchedulerManager( ScheduleManagerInterface * _scheduler ) = 0;

    public:
        virtual void setRenderCamera( RenderCameraInterface * _camera) = 0;
        virtual const RenderCameraInterface * getRenderCamera() const = 0;

	public:
		virtual void setRenderViewport( RenderViewportInterface * _renderViewport ) = 0;
		virtual const RenderViewportInterface * getRenderViewport() const = 0;
        
    public:
        virtual MousePickerSystemInterface * getMousePickerSystem() const = 0;
        virtual GlobalHandleSystemInterface * getGlobalHandleSystem() const = 0;

    public:
        virtual ScheduleManagerInterface * getScheduleManager() const = 0;
        virtual ScheduleManagerInterface * getScheduleManagerGlobal() const = 0;

	public:
		virtual ScheduleManagerInterface * createTimingManager() = 0;
		virtual bool destroyTimingManager( ScheduleManagerInterface * _timing ) = 0;

    public:
		virtual ScheduleManagerInterface * getTimingManager() const = 0;
		virtual ScheduleManagerInterface * getTimingManagerGlobal() const = 0;

	public:
		virtual Affectorable * getAffectorable() const = 0;
		virtual Affectorable * getAffectorableGlobal() const = 0;
		
    public:
        virtual Join * addJoin( Node * _left, Node * _right, const mt::vec2f & _offset ) = 0;
        virtual void removeJoin( Join * _join ) = 0;
        virtual bool isJoin( Node * _left, Node * _right ) const = 0;

        virtual void getJoins( Node * _node, TVectorNode & _joins ) const = 0;
		
    public:
        virtual void toggleDebugText() = 0;
    };

#   define PLAYER_SERVICE( serviceProvider )\
    ((Menge::PlayerServiceInterface*)SERVICE_GET(serviceProvider, Menge::PlayerServiceInterface))
}