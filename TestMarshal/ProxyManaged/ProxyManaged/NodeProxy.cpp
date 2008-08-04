#include <STDIO.H>									// needed for NULL

#include "NodeProxy.h"
#include "UnmanagedDLL/Node.h"

using namespace System::Runtime::InteropServices;	// needed for Marshal

NodeProxy::NodeProxy()
: m_pUnmanagedTestClass(NULL)
{
	this->m_pUnmanagedTestClass = new Node();
}

NodeProxy::~NodeProxy()
{
	delete this->m_pUnmanagedTestClass;
	this->m_pUnmanagedTestClass = NULL;
}

Boolean NodeProxy::compile()
{
	return this->m_pUnmanagedTestClass->compile();
	//return false;
}