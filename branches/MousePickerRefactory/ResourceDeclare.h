#	pragma once

#	include "Config/Typedef.h"

#	define RESOURCE_DECLARE(C)\
	public:\
	static Factory * genFactory();\
	void accept( ResourceVisitor * _visitor ) override;\
	private:
