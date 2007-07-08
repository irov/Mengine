#	pragma once

#	include <string>

namespace Menge
{
	class SystemDLLImpl
	{
	public:
		SystemDLLImpl( const std::string &_filename );
		~SystemDLLImpl();

	protected:
		void * m_hInstance;

		typedef int (*FInterface)();

		FInterface m_init;
		FInterface m_fini;
	};

	template<class T>
	class SystemDLL
		: public SystemDLLImpl
	{
	public:
		typedef bool (*FInterfaceInitial)(T**);
		typedef void (*FInterfaceRelease)(T*);

		SystemDLL(const std::string &_filename )
			: SystemDLLImpl( _filename )
		{
			((FInterfaceInitial)m_init)(&m_interface);
		}

		~SystemDLL()
		{
			((FInterfaceRelease)m_fini)(m_interface);
		}

	protected:
		T * m_interface;
	};
}