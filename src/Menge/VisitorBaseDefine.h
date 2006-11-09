#	pragma once

#	undef VISITABLE_CLASS
#	define VISITABLE_CLASS( X ) class X;

namespace Menge
{
#	include "VisitorClassDeclare.h"
}

#	undef VISITABLE_CLASS
#	define VISITABLE_CLASS( X ) virtual void call( X *_node){}