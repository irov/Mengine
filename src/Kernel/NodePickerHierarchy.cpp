#include "NodePickerHierarchy.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool hasNodePickerInheritance( const Node * _node )
        {
            if( _node == nullptr )
            {
                return false;
            }

            const PickerInterface * picker = _node->getPicker();

            if( picker != nullptr )
            {
                return true;
            }

            const Node * parent = _node->getParent();

            bool successful = Helper::hasNodePickerInheritance( parent );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        PickerInterface * getNodePickerInheritance( Node * _node, Node ** const _pickerNode )
        {
            if( _node == nullptr )
            {
                return nullptr;
            }

            PickerInterface * picker = _node->getPicker();

            if( picker != nullptr )
            {
                *_pickerNode = _node;

                return picker;
            }

            Node * parent = _node->getParent();

            PickerInterface * picker_parent = Helper::getNodePickerInheritance( parent, _pickerNode );

            return picker_parent;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}