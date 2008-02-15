#	pragma once

#	include "Interface/ApplicationInterface.h"

#	include <list>

namespace Menge
{
	class Application;
	class SystemDLL;
}

class WinApplication
	: public ApplicationInterface
{
public:
	WinApplication();
	~WinApplication();

public:
	bool init( const char * _xmlFile ) override;
	void run() override;

protected:
	Menge::Application * m_application;

	typedef std::list<Menge::SystemDLL *> TListApplicationDLL;
	TListApplicationDLL m_listApplicationDLL;
};