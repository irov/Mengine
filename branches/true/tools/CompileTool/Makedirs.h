#	pragma once
#	include <string>
#	include <functional>

class Callback;
class Compiler;

class Makedirs
	: public std::binary_function<std::string, std::string, bool>
{
public:
	Makedirs( Callback * _callback, Compiler * _compiler );
	bool operator()( const std::string & _src, const std::string & _dst ) const;

private:
	Callback * m_callback;
	Compiler * m_compiler;
};