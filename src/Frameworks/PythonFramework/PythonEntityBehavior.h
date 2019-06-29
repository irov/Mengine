#pragma once

#include "Kernel/Entity.h"

#include "pybind/object.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PythonEntityBehavior
        : public EntityBehaviorInterface
        , public Factorable
    {
    public:
        PythonEntityBehavior();
        ~PythonEntityBehavior() override;

    public:
        void setScriptObject( const pybind::object & _obj );
        const pybind::object & getScriptObject() const;

    public:
        template<class ... Args>
        void call( const pybind::object & _cb, Args && ... _args )
        {
            _cb.call( m_obj, std::forward<Args>( _args ) ... );
        }

    protected:
        pybind::object m_obj;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PythonEntityBehavior, EntityBehaviorInterface> PythonEntityBehaviorPtr;
    //////////////////////////////////////////////////////////////////////////
}