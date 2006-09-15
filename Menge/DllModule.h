#	pragma once

#	include <string>

namespace RvEngine
{
	class DllModule
	{
		typedef int (*FProc)();
	public:
		DllModule();
		DllModule(const std::string &_fileDll);
		~DllModule();

	public:
		bool initDllModule(const std::string &_fileDll);

		FProc getInitFunction()const;
		FProc getReleaseFunction()const;

	private:
		void *m_hInst;
	};


	template<class T>
	class DllModuleInterface
		: public DllModule
	{
		typedef bool (*FInerfaceInitialize)(T **);
		typedef void (*FInerfaceRelize)(T *);

	public:
		DllModuleInterface()
			: DllModule()
			, m_interface(0)
			, m_funcInterfaceRelize(0)
		{		
		}

		DllModuleInterface(const std::string &_fileDll)
			: DllModule(_fileDll)
			, m_interface(0)
		{	
			FInerfaceInitialize initialize = 
				(FInerfaceInitialize)getInitFunction();

			m_funcInterfaceRelize = 
				(FInerfaceRelize)getReleaseFunction();

			if( initialize && m_funcInterfaceRelize )
			{
				initialize(&m_interface);
			}
		}

		~DllModuleInterface()
		{
			if( m_funcInterfaceRelize )
			{
				m_funcInterfaceRelize(m_interface);
			}
		}

	public:
		T * getInterface(){return m_interface;}
		const T * getInterface()const{return m_interface;}

	protected:
		T * m_interface;

	private:
		FInerfaceRelize m_funcInterfaceRelize;
	};
}