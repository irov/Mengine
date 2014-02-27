#	pragma once

#   include "Factory/Factorable.h"

#   include "stdex/intrusive_ptr.h"

#   include <stddef.h>

namespace Menge
{
	class FactorablePtr
        : public Factorable
	{
	public:
		FactorablePtr();
		~FactorablePtr();

	private:
		void destroy() override;

	public:
        size_t getReference() const;

    protected:
        friend void intrusive_ptr_add_ref( FactorablePtr * _ptr );
        friend void intrusive_ptr_dec_ref( FactorablePtr * _ptr );

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
    inline void intrusive_ptr_dec_ref( FactorablePtr * _ptr )
    {
        if( --_ptr->m_reference == 0 )
        {
            _ptr->destroy();
        }
    }
}
