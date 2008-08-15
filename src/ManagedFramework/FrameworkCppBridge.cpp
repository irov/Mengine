#	include "windows.h"

#	include <STDIO.H>									
#	include "FrameworkCppBridge.h"

#	include "Framework/Framework.h"

using namespace System::Runtime::InteropServices;	

ManagedSprite::ManagedSprite(MengeSprite * _mengeSprite)
: m_pUnmanagedTestClass(_mengeSprite)
{
	//_mengeSprite->
	//this->m_pUnmanagedTestClass = new MengeSprite(0);
}

bool ManagedSprite::activate()
{
	return this->m_pUnmanagedTestClass->activate();
}

void ManagedSprite::setImageIndex( int _index )
{
	this->m_pUnmanagedTestClass->setImageIndex(_index);
}
int ManagedSprite::getImageIndex()
{
	return this->m_pUnmanagedTestClass->getImageIndex();
}
void ManagedSprite::setImageResource( String^ _name )
{

}
int ManagedSprite::getImageCount()
{
	return this->m_pUnmanagedTestClass->getImageCount();
}
bool ManagedSprite::getCenterAlign()
{
	return this->m_pUnmanagedTestClass->getCenterAlign();
}
void ManagedSprite::flip( bool _x )
{
	this->m_pUnmanagedTestClass->flip(_x);
}
void ManagedSprite::setAlpha( float _alpha )
{
	this->m_pUnmanagedTestClass->setAlpha(_alpha);
}
void ManagedSprite::alphaTo( float _alpha, float _time )
{
	this->m_pUnmanagedTestClass->alphaTo(_alpha,_time);
}

MengeSprite * ManagedSprite::getMengeSprite()
{
	return m_pUnmanagedTestClass;
}

void ManagedSprite::setPos( float X, float Y )
{
	m_pUnmanagedTestClass->setPos(X,Y);
}

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

void ManagedFramework::initMenge()
{
	this->m_pUnmanagedTestClass->initMenge();
}

void ManagedFramework::releaseMenge()
{
	this->m_pUnmanagedTestClass->releaseMenge();
}

void ManagedFramework::renderLine(unsigned int _color, float X, float Y, float DX, float DY)
{
	this->m_pUnmanagedTestClass->renderLine(_color,X,Y,DX,DY);
}

void ManagedFramework::setCurrentScene(String^ strValue)
{
	IntPtr pString = Marshal::StringToHGlobalAnsi(strValue);
	try
	{
		char* pchString = static_cast<char *>(pString.ToPointer());
		this->m_pUnmanagedTestClass->setCurrentScene(pchString);
	}
	finally
	{
		Marshal::FreeHGlobal(pString);
	}
}


void ManagedFramework::layerAppend(String^ strValue, ManagedSprite ^ _sprite)
{
	IntPtr pString = Marshal::StringToHGlobalAnsi(strValue);
	try
	{
		char* pchString = static_cast<char *>(pString.ToPointer());
		MengeSprite * mengeSprite = _sprite->getMengeSprite();
		this->m_pUnmanagedTestClass->layerAppend(pchString,mengeSprite);
	}
	finally
	{
		Marshal::FreeHGlobal(pString);
	}	
}

ManagedSprite ^ ManagedFramework::createSprite(String^ strValue, float x, float y)
{
	IntPtr pString = Marshal::StringToHGlobalAnsi(strValue);
	try
	{
		char* pchString = static_cast<char *>(pString.ToPointer());
		MengeSprite * mengeSprite = this->m_pUnmanagedTestClass->createSprite(pchString,x,y);

		ManagedSprite ^ managedSprite = gcnew ManagedSprite(mengeSprite);

		return managedSprite;
	}
	finally
	{
		Marshal::FreeHGlobal(pString);
	}
}

void ManagedFramework::createWindow(IntPtr hWnd)
{
	//IntPtr pString = Marshal::StringToHGlobalAnsi(_title);
	HWND handle = static_cast<HWND> (hWnd.ToPointer());
		//char * pchString = static_cast<char *>(pString.ToPointer());
	this->m_pUnmanagedTestClass->createWindow(handle);		
}

void ManagedFramework::onUpdate(float _timing)
{
	this->m_pUnmanagedTestClass->onUpdate(1000);		
}