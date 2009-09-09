#	pragma once

#	include "Callback.h"

#	include "Menge/Application.h"

class CopyFileCallback
	: public Callback
{
public:
	CopyFileCallback();
	~CopyFileCallback();

public:

	bool execute( const std::string & _src, const std::string & _dst, Compiler * );

private:
};
