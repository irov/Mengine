#	pragma once

#	include "Interface/ApplicationInterface.h"

#	include <string>

class WinSystemDLL 
	: public Menge::SystemDLLInterface
{
public:
	WinSystemDLL( const std::string & _filename );
	virtual ~WinSystemDLL();

protected:
	std::string m_filename;
	void * m_hInstance;

};