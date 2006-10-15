#	include "DllModule.h"

#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	DllModule::DllModule()
		: m_hInst(0)
	{

	}
	DllModule::DllModule(const std::string &_fileDll)
	{
		m_hInst = LoadLibrary(_fileDll.c_str());
	}
	//////////////////////////////////////////////////////////////////////////
	DllModule::~DllModule()
	{
		FreeLibrary((HMODULE)m_hInst);
	}
	//////////////////////////////////////////////////////////////////////////
	bool DllModule::initDllModule(const std::string &_fileDll)
	{
		m_hInst = LoadLibrary(_fileDll.c_str());

		if( m_hInst == 0 )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	DllModule::FProc DllModule::getInitFunction()const
	{
		return (DllModule::FProc)GetProcAddress((HMODULE)m_hInst, "initInterfaceSystem" );
	}
	//////////////////////////////////////////////////////////////////////////
	DllModule::FProc DllModule::getReleaseFunction()const
	{
		return (DllModule::FProc)GetProcAddress((HMODULE)m_hInst, "releaseInterfaceSystem" );
	}
}

