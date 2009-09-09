#	pragma once

#	include <string>

class Compiler;

class Callback
{
public:
	virtual ~Callback(){};
	virtual bool execute( const std::string & _src, const std::string & _dst, Compiler * ) = 0;
};