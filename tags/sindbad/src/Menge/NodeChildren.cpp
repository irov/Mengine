
#	include "NodeChildren.h"
#	include "LogEngine.h"

namespace Menge
{	
	namespace Helper
	{
		//////////////////////////////////////////////////////////////////////////
		void errorMessageChildrenIncorrectType( Node * _parent, Node *_children )
		{
			MENGE_LOG("Node '%s' type '%s' addChildren failed '%s' type '%s' is incorrect type"
				, _parent->getName().c_str()
				, _parent->getType().c_str()
				, _children->getName().c_str()
				, _children->getType().c_str()
				);
		}
		//////////////////////////////////////////////////////////////////////////
		void errorMessageChildrenAlreadyExsist( Node * _parent, Node * _children )
		{
			MENGE_LOG("Node '%s' type '%s' addChildren failed '%s' type '%s' is already exsist"
				, _parent->getName().c_str()
				, _parent->getType().c_str()
				, _children->getName().c_str()
				, _children->getType().c_str()
				);
		}
		//////////////////////////////////////////////////////////////////////////
		void errorMessageParentIncorrectType( Node * _current, Node * _parent )
		{
			MENGE_LOG("Node '%s' type '%s' setParent failed '%s' type '%s' is incorrect type"
				, _current->getName().c_str()
				, _current->getType().c_str()
				, _parent->getName().c_str()
				, _parent->getType().c_str()
				);
		}
	}
}
