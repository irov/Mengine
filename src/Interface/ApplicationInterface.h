#	pragma once

class ApplicationInterface
{
public:
	virtual bool init( const char * _xmlFile ) = 0;
	virtual void run() = 0;
};

bool initInterfaceSystem(ApplicationInterface **);
void releaseInterfaceSystem(ApplicationInterface *);