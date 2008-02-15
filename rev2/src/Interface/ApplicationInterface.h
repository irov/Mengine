#	pragma once

class ApplicationInterface
{
public:
	virtual bool init( const char * _xmlFile, const char * _args ) = 0;
	virtual void run() = 0;
};

bool initInterfaceSystem( ApplicationInterface ** );
void releaseInterfaceSystem( ApplicationInterface * );