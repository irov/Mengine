#	pragma once

#	include "Interface/ApplicationInterface.h"

//#	include <string>

class WinSystemDLL 
	: public Menge::SystemDLLInterface
{
public:
	WinSystemDLL( const Menge::String& _filename );
	virtual ~WinSystemDLL();

protected:
	Menge::String m_filename;
	void * m_hInstance;

};
