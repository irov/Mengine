
#include "D3D7RenderSystem.h"
#include "DDDriverList.h"
#include "DDDriver.h"
//#include "OgreLogManager.h"
//#include "OgreException.h"

namespace Menge
{
    // Non-member callback functions
    static BOOL WINAPI DDEnumCallbackEx(GUID FAR *lpGUID,
                    LPSTR     lpDriverDescription,
                    LPSTR     lpDriverName,
                    LPVOID    lpContext,
                    HMONITOR  hm)
    {

        DDDriverList* driverList;

        // Use context pointer to add this new object to the list
        driverList = (DDDriverList*)lpContext;
        driverList->addDriver(lpGUID, lpDriverDescription, lpDriverName);


        // Continue enumeration (to find other cards)
        return TRUE;
    }
	//////////////////////////////////////////////////////////////////////////
    static BOOL WINAPI DDEnumCallback(GUID FAR *lpGUID,
                               LPSTR  lpDriverDescription,
                               LPSTR  lpDriverName,
                               LPVOID lpContext)
    {
        return DDEnumCallbackEx(lpGUID,lpDriverDescription,lpDriverName,lpContext,NULL);
    }
	//////////////////////////////////////////////////////////////////////////
    DDDriverList::DDDriverList( LogSystemInterface* _logInterface )
		: m_logInterface( _logInterface )
    {
        // Default constructor
        // Will create a  new driver list and enumerate it

        // Enumerate the list
        enumerate();
    }
	//////////////////////////////////////////////////////////////////////////
    DDDriverList::~DDDriverList()
    {
		for( std::size_t i=0; i < count(); i++ )
		{
			item(i)->cleanup();
		}
        // Release drivers
        m_driverList.clear();
    }
	//////////////////////////////////////////////////////////////////////////
    BOOL DDDriverList::enumerate()
    {
        LPDIRECTDRAWENUMERATEEX lpDDEnumEx;
        HINSTANCE h;

        m_logInterface->logMessage( "----- DirectDraw Detection Starts" );

        h = LoadLibrary("ddraw.dll");

        // If ddraw.dll doesn't exist in the search path,
        // then DirectX probably isn't installed, so fail.
        assert( h && "DDDriverList - enumerate -> Error loading ddraw.dll" );

        // Note that you must know which version of the
        // function to retrieve (see the following text).
        // For this example, we use the ANSI version.
        lpDDEnumEx = (LPDIRECTDRAWENUMERATEEX) GetProcAddress(h,"DirectDrawEnumerateExA");
         // If the function is there, call it to enumerate all display
        // devices attached to the desktop, and any non-display DirectDraw
        // devices.
        if (lpDDEnumEx)
            lpDDEnumEx(DDEnumCallbackEx, this,
                DDENUM_ATTACHEDSECONDARYDEVICES |
                DDENUM_NONDISPLAYDEVICES             );
        else
        {
            /*
             * We must be running on an old version of DirectDraw.
             * Therefore MultiMon isn't supported. Fall back on
             * DirectDrawEnumerate to enumerate standard devices on a
             * single-monitor system.
            */
            DirectDrawEnumerate(DDEnumCallback,this);

        }
        // If the library was loaded by calling LoadLibrary(),
        // then you must use FreeLibrary() to let go of it.

        FreeLibrary(h);

        m_logInterface->logMessage( "----- DirectDraw Detection Ends" );

        return TRUE;
    }
	//////////////////////////////////////////////////////////////////////////
    void DDDriverList::addDriver(
        GUID FAR *lpGuid,
        LPSTR lpDriverDesc,
        LPSTR lpDriverName )
    {
        m_driverList.push_back( DDDriver(lpGuid, lpDriverDesc, lpDriverName) );

        m_logInterface->logMessage( String( String("Detected DirectDraw driver ") + lpDriverDesc ).c_str() );
    }
	//////////////////////////////////////////////////////////////////////////
	std::size_t DDDriverList::count() const
    {
        return m_driverList.size();
    }
	//////////////////////////////////////////////////////////////////////////
    DDDriver* DDDriverList::item( size_t _index )
    {
        return &m_driverList.at( _index );
    }
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
