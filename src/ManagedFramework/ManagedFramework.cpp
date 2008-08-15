#	include "ManagedFramework.h"

using namespace System::Runtime::InteropServices;	// needed for Marshal

ManagedFramework::ManagedFramework()
{
	app->createRenderWindow(0);
}

ManagedFramework::~ManagedFramework()
{

}