#	pragma once

//#	include <string>
#	include "Config/Typedef.h"
#	include "Factory.h"

struct FactoryGenStruct
{
	Menge::String type;
};

#	define OBJECT_DECLARE(C)\
	public:\
	static Node * genObjectNew( const FactoryGenStruct &);\
	static Node * genObjectPlacement( void*, const FactoryGenStruct &);\
	static Factory<String, Node *, FactoryGenStruct>::PoolManager<C> ms_poolManager;\
	private:
