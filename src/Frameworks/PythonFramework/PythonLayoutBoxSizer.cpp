#include "PythonLayoutBoxSizer.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PythonLayoutBoxSizer::PythonLayoutBoxSizer()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonLayoutBoxSizer::~PythonLayoutBoxSizer()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonLayoutBoxSizer::onGetLayoutBoxSize( mt::vec2f * const _size ) const
    {
        mt::vec2f new_size = this->call_cb();

        if( _size->x == new_size.x && _size->y == new_size.y )
        {
            return false;
        }

        *_size = new_size;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
