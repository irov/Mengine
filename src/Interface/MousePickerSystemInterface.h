#   pragma once

#	include "Interface/InputSystemInterface.h"
#	include "Interface/ScriptSystemInterface.h"

#   include "Math/vec2.h"

namespace Menge
{
    class Arrow;

    class MousePickerTrapInterface
        : virtual public InputSystemHandler
    {
    public:
        virtual bool pick( const mt::vec2f& _point, Arrow * _arrow ) = 0;

    public:
        virtual bool onMouseEnter() = 0;
        virtual void onMouseLeave() = 0;

    public:
        virtual PyObject * getPickerEmbed() = 0; 

    public:
        virtual bool isPickerActive() const = 0;        
    };

    typedef std::vector<MousePickerTrapInterface *> TVectorPickerTraps;

    struct PickerTrapState
    {
        MousePickerTrapInterface * trap;
        size_t id;
        bool picked;
        bool handle;
        bool dead;
    };

    class MousePickerSystemInterface
        : public InputSystemHandler
    {
    public:
        virtual void block( bool _value ) = 0;

    public:
        virtual void setArrow( Arrow * _arrow ) = 0;

    public:
        virtual void update() = 0;
        virtual void clear() = 0;

    public:
        virtual void pickTrap( const mt::vec2f& _point, TVectorPickerTraps & _traps ) = 0;

    public:
        virtual PickerTrapState * regTrap( MousePickerTrapInterface * _trap ) = 0;
        virtual void unregTrap( PickerTrapState * _id ) = 0;

    public:
        virtual void beginTrap() = 0;
        virtual void updateTrap( PickerTrapState * _id ) = 0;

    public:
        virtual size_t getPickerTrapCount() const = 0;

    public:
        virtual void handleMouseEnter( const mt::vec2f & _point ) = 0;
        virtual void handleMouseLeave() = 0;
    };
}