#	pragma once

#	include "Config/Typedef.h"
#	include "Factory.h"

struct FactoryGenStruct
{
	Menge::String type;
};

#	define OBJECT_DECLARE(C)\
	public:\
	static void * genObject( void * _data );\
	private:
