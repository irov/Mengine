# pragma once

using namespace System;

// Forward declaration
class Framework;

public ref class ManagedFramework
{
public:
	ManagedFramework();
	virtual ~ManagedFramework();
	void createWindow(IntPtr hWnd);
	void step();
	//void createWindow(String ^ _title, int _width, int _height, bool _isfullscreen);

private:
	Framework * m_pUnmanagedTestClass;
};