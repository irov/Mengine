#include "PythonLayoutElementGetter.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PythonLayoutElementGetter::PythonLayoutElementGetter()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonLayoutElementGetter::~PythonLayoutElementGetter()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonLayoutElementGetter::onGetLayoutElementSize( float * const _size ) const
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