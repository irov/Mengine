#include "MovieNodeExtra.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MovieNodeExtra::MovieNodeExtra()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MovieNodeExtra::~MovieNodeExtra()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieNodeExtra::movieForwardUpdate( float _time, uint32_t _beginFrame, uint32_t _endFrame, const MovieLayer & _layer )
    {
        this->_movieForwardUpdate( _time, _beginFrame, _endFrame, _layer );
    }
}