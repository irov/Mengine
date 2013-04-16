#   pragma once

#   include "Interface/ServiceInterface.h"
#   include "Interface/TimingManagerInterface.h"
#   include "Interface/ScheduleManagerInterface.h"
#   include "Interface/MousePickerSystemInterface.h"
#   include "Interface/GlobalHandleSystemInterface.h"
#   include "Interface/NodeInterface.h"
#   include "Interface/ScriptSystemInterface.h"

namespace Menge
{
    class Join;
    class Node;
    class Scene;
    class Arrow;
    class Camera2D;
    
    class PlayerServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE("PlayerService")

    public:
        virtual bool setCurrentScene( const ConstString& _scene, bool _destroyOld, bool _destroyAfterSwitch, PyObject* _cb ) = 0;
        virtual bool removeCurrentScene( PyObject * _cb ) = 0;
        virtual Scene * getCurrentScene() = 0;

        virtual float getTime() const = 0;

    public:
        virtual void setArrow( Arrow * _arrow ) = 0;
        virtual Arrow * getArrow() const = 0;

    public:
        virtual void setRenderCamera2D( Camera2D * _camera) = 0;
        virtual Camera2D * getRenderCamera2D() const = 0;
        
    public:
        virtual MousePickerSystemInterface * getMousePickerSystem() const = 0;
        virtual GlobalHandleSystemInterface * getGlobalHandleSystem() const = 0;

    public:
        virtual ScheduleManagerInterface * getScheduleManager() const = 0;
        virtual ScheduleManagerInterface * getScheduleManagerGlobal() const = 0;

    public:
        virtual TimingManagerInterface * getTimingManager() const = 0;
        virtual TimingManagerInterface * getTimingManagerGlobal() const = 0;

    public:
        virtual Join * addJoin( Node * _left, Node * _right, const mt::vec2f & _offset ) = 0;
        virtual void removeJoin( Join * _join ) = 0;
        virtual bool isJoin( Node * _left, Node * _right ) const = 0;

        virtual void getJoins( Node * _node, TVectorNode & _joins ) const = 0;
    };

#   define PLAYER_SERVICE( serviceProvider )\
    (Menge::getService<Menge::PlayerServiceInterface>(serviceProvider))
}