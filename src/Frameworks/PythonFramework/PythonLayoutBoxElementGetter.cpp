#include "PythonLayoutBoxElementGetter.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PythonLayoutBoxElementGetter::PythonLayoutBoxElementGetter()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonLayoutBoxElementGetter::~PythonLayoutBoxElementGetter()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonLayoutBoxElementGetter::onGetLayoutBoxElementSize( float * const _size ) const
    {
        float new_size = this->call_cb();

        if( *_size == new_size )
        {
            return false;
        }

        *_size = new_size;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
