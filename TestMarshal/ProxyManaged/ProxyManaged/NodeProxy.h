#	pragma once

using namespace System;

class Node;

public ref class NodeProxy
{
public:
	NodeProxy();
	virtual ~NodeProxy();

	Boolean compile();

private:
	Node * m_pUnmanagedTestClass;
};