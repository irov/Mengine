#	pragma once

#   include "Utils/Factory/Factorable.h"

#   include <stddef.h>

namespace Menge
{
	class FactorablePtr
        : public Factorable
	{
	public:
		FactorablePtr();
		~FactorablePtr();

    public:
        size_t getReference() const;

    protected:
        friend void intrusive_ptr_add_ref( FactorablePtr * _ptr );
        friend void intrusive_ptr_release( FactorablePtr * _ptr );

#   ifdef _DEBUG
    protected:
        void _checkDestroy() override;
#   endif

    protected:
        size_t m_reference;
	};
    //////////////////////////////////////////////////////////////////////////
    inline void intrusive_ptr_add_ref( FactorablePtr * _ptr )
    {
        ++_ptr->m_reference;
    }
    //////////////////////////////////////////////////////////////////////////
    inline void intrusive_ptr_release( FactorablePtr * _ptr )
    {
        if( --_ptr->m_reference == 0 )
        {
            _ptr->destroy();
        }
    }
}
