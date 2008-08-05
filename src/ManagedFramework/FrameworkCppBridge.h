#ifndef __TestClassDLLBridge_h__
#define __TestClassDLLBridge_h__

using namespace System;

// Forward declariation
class Framework;

public ref class ManagedFramework
{
public:
	ManagedFramework();
	virtual ~ManagedFramework();
	void createWindow(IntPtr hWnd);
	//void createWindow(String ^ _title, int _width, int _height, bool _isfullscreen);

private:
	Framework * m_pUnmanagedTestClass;
};


#endif	// __TestClassDLLBridge_h__
