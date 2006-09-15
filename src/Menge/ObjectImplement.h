#	pragma once

#	include "VisitorBase.h"
#	include "NodeFactory.h"

using namespace RvEngine;


#	ifndef OBJECT_USER_VISITOR
#	define OBJECT_IMPLEMENT_VISITOR(C)\
	void C::visit(Visitor::Base *_visitor)\
	{\
	_visitor->execute(this);\
	}
#	else
#	define OBJECT_IMPLEMENT_VISITOR(C)
#	endif


#	ifndef OBJECT_USER_GENERATOR
#	define OBJECT_IMPLEMENT_GENERATOR(C)\
	Node * C::genObject(SceneManager *_sceneManager, const std::string &_type){\
	Node *node = new C();\
	node->setType(_type);\
	node->setSceneManager(_sceneManager);\
	return node;\
	}
#	else
#	define OBJECT_IMPLEMENT_GENERATOR(C)
#	endif


#	define OBJECT_IMPLEMENT_FACTORY(C)\
	OBJECT_IMPLEMENT_GENERATOR(C)\
	namespace {\
	volatile static NodeFactory::\
	Auto _classRegistration(#C,C::genObject);\
	}

#	define OBJECT_IMPLEMENT(C)\
	OBJECT_IMPLEMENT_VISITOR(C)\
	OBJECT_IMPLEMENT_FACTORY(C)



