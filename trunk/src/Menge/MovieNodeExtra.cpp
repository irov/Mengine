#	include "MovieNodeExtra.h"

namespace Menge
{
    void MovieNodeExtra::movieForwardUpdate( float _time, size_t _beginFrame, size_t _endFrame, const MovieLayer & _layer )
    {
        this->_movieForwardUpdate( _time, _beginFrame, _endFrame, _layer );
    }
}