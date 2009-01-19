#	pragma once

#	include "ObjectDeclare.h"
#	include "ResourceDeclare.h"

#	include "Node.h"
#	include "ResourceReference.h"

#	include "SceneManager.h"
#	include "ResourceManager.h"

#	include <string>

#	define OBJECT_FACTORY( Type )\
	Holder<SceneManager>::hostage()\
		->registrationType( #Type, &Type::genObject )

#	define RESOURCE_FACTORY( Type )\
	Holder<ResourceManager>::hostage()\
		->registrationType( #Type , &Type::genObject )
