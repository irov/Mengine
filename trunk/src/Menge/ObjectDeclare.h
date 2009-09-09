#	pragma once

#	include "Config/Typedef.h"

#	define FACTORY_DECLARE( Class )\
	public:\
	static Factory * genFactory();\
	private:
