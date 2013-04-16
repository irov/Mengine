#	include "Kernel/Loadable.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
    bool Loadable::loader( const Metabuf::Metadata * _parser )
    {
        bool result = this->_loader( _parser );
        
        
        return result;
    }
}