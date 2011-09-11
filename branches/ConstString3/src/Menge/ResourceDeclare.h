#	pragma once

#	include "Config/Typedef.h"

//static Factory * genFactory();\

#	define RESOURCE_DECLARE(C)\
	public:\
	void accept( ResourceVisitor * _visitor ) override;\
	private:
