#	pragma once

#	include "ResourceDeclare.h"
#	include "ResourceVisitor.h"

#	define RESOURCE_IMPLEMENT( Class )\
	void Class::accept( ResourceVisitor * _visitor ){ _visitor->visit(this); }

