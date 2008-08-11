#	include "windows.h"

#	include <STDIO.H>									
#	include "FrameworkCppBridge.h"

#	include "Framework/Framework.h"


using namespace System::Runtime::InteropServices;	


ManagedFramework::ManagedFramework()
	: m_pUnmanagedTestClass(NULL)
{
	this->m_pUnmanagedTestClass = new Framework();
}

ManagedFramework::~ManagedFramework()
{
	delete this->m_pUnmanagedTestClass;
	this->m_pUnmanagedTestClass = NULL;
}

/*void ManagedFramework::createWindow(String ^ _title, int _width, int _height, bool _isfullscreen)
{
	IntPtr pString = Marshal::StringToHGlobalAnsi(_title);
	try
	{
		char * pchString = static_cast<char *>(pString.ToPointer());
		this->m_pUnmanagedTestClass->createWindow(pchString, _width, _height, _isfullscreen);		
	}
	finally
	{
		Marshal::FreeHGlobal(pString);
	}
}*/

void ManagedFramework::createWindow(IntPtr hWnd)
{
	//IntPtr pString = Marshal::StringToHGlobalAnsi(_title);
	HWND handle = static_cast<HWND> (hWnd.ToPointer());
		//char * pchString = static_cast<char *>(pString.ToPointer());
	this->m_pUnmanagedTestClass->createWindow(handle);		
}

void ManagedFramework::step(float _timing)
{
	this->m_pUnmanagedTestClass->step(1000);		
}
