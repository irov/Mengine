#include "NodePickerHierarchy.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        PickerInterface * getNodePickerInheritance( Node * _node )
        {
            if( _node == nullptr )
            {
                return nullptr;
            }

            PickerInterface * picker = _node->getPicker();

            if( picker != nullptr )
            {
                return picker;
            }

            Node * parent = _node->getParent();

            PickerInterface * picker_parent = Helper::getNodePickerInheritance( parent );

            return picker_parent;
        }
    }
}