
#	include "NodeChildren.h"
#	include "LogEngine.h"

namespace Menge
{	
	namespace Helper
	{
		void errorMessageChildrenIncorrectType( Node * _parent, Node *_children )
		{
			MENGE_LOG("Node '%s' type '%s' addChildren failed '%s' is incorrect type '%s'"
				, _parent->getName().c_str()
				, _parent->getType().c_str()
				, _children->getName()
				, _children->getType()
				);
		}
	}
}
