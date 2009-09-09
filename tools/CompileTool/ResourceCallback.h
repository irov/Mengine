#	pragma once

#	include "Callback.h"

class ResourceCallback : public Callback
{
public:
	ResourceCallback();
	bool execute( const std::string & _src, const std::string & _dst, Compiler * );
};