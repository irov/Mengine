#	pragma once

#	include "Config/Typedef.h"

#	define RESOURCE_DECLARE(C)\
	public:\
    void accept( Menge::ResourceVisitor * _visitor ) override;\
	private:
