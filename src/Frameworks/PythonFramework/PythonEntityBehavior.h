#pragma once

#include "Environment/Python/PythonIncluder.h"

#include "Kernel/Entity.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PythonEntityBehavior
        : public EntityBehaviorInterface
    {
        DECLARE_FACTORABLE( PythonEntityBehavior );

    public:
        PythonEntityBehavior();
        ~PythonEntityBehavior() override;

    public:
        bool initialize() override;
        void finalize() override;

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