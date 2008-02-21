#	pragma once

#	include <string>

class SystemDLL
{
public:
	SystemDLL( const std::string & _filename );
	virtual ~SystemDLL();

protected:
	std::string m_filename;
	void * m_hInstance;

	typedef int (*FInterface)();

	FInterface m_init;
	FInterface m_fini;
};

template<class T>
class SystemInterfaceDLL
	: public SystemDLL
{
public:
	typedef bool (*FInterfaceInitial)(T**);
	typedef void (*FInterfaceRelease)(T*);

	SystemInterfaceDLL(const std::string &_filename )
		: SystemDLL( _filename )
	{
		((FInterfaceInitial)m_init)(&m_interface);
	}

	~SystemInterfaceDLL()
	{
		((FInterfaceRelease)m_fini)(m_interface);
	}

	T * getInterface()
	{
		return m_interface;
	}

protected:
	T * m_interface;
};