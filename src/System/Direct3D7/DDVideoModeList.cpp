
#include "DDVideoModeList.h"
#include "DDVideoMode.h"

#	include <cassert>

//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    // Non-member callback functions
    static HRESULT CALLBACK DDEnumModesCallback( LPDDSURFACEDESC2 lpDDSD2, LPVOID lpContext )
    {
        DDVideoModeList* modeList;

        // Omit any palettised modes
        if (lpDDSD2->ddpfPixelFormat.dwRGBBitCount >= 16)
        {

            modeList = (DDVideoModeList*) lpContext;

            modeList->AddMode(lpDDSD2);
        }

        // Continue enumeration
        return DDENUMRET_OK;
    }
	//////////////////////////////////////////////////////////////////////////
    DDVideoModeList::DDVideoModeList( LPDIRECTDRAW7 directDraw )
    {
        // Will create a  new driver list and enumerate it
        assert( directDraw && "DDVideoModeList Contructor -> NULL supplied as DD interface pointer." );

        lpDD7 = directDraw;
        // Enumerate the list
        enumerate();
    }
	//////////////////////////////////////////////////////////////////////////
    DDVideoModeList::~DDVideoModeList()
    {
        // Release each video mode object
        std::vector<DDVideoMode>::iterator p = m_modeList.begin();

        m_modeList.erase(p, p+m_modeList.size()-1);
    }
	//////////////////////////////////////////////////////////////////////////
    BOOL DDVideoModeList::enumerate()
    {
        HRESULT hr;

        // Enumerate display modes
        // Different refresh rates are NOT enumerated (dwFlags = 0)
        hr = lpDD7->EnumDisplayModes(0, NULL, this, DDEnumModesCallback);
        assert( SUCCEEDED(hr) && "DDVideoModeList - enumerate -> Error enumerating display modes" );

        return TRUE;
    }
	//////////////////////////////////////////////////////////////////////////
    void DDVideoModeList::AddMode( LPDDSURFACEDESC2 lpDDSD2 )
    {
        // Add it to my list
        m_modeList.push_back(DDVideoMode(lpDDSD2));
    }
	//////////////////////////////////////////////////////////////////////////
    unsigned int DDVideoModeList::count()
    {
        return static_cast< unsigned int >( m_modeList.size() );
    }
	//////////////////////////////////////////////////////////////////////////
    DDVideoMode* DDVideoModeList::item( int _index )
    {
        // Get an iterator for the vector
        std::vector<DDVideoMode>::iterator p = m_modeList.begin();

        // Return the indexed driver
        return &p[_index];
    }
	//////////////////////////////////////////////////////////////////////////
    DDVideoMode* DDVideoModeList::getClosestMatch( int _width, int _height, int _colourDepth )
    {
        // Search through looking for closest match
        int bestDiff, currentDiff, bestIndex;
        DDVideoMode *vm;

        std::vector<DDVideoMode>::iterator p = m_modeList.begin();


        bestDiff = 9999;
        bestIndex = -1;
        for( unsigned j = 0; j < count(); j++ )
        {
            currentDiff = 0;
            vm = &p[j];
            currentDiff += abs(vm->m_width - _width);
            currentDiff += abs(vm->m_height - _height);
            currentDiff += abs(vm->m_colourDepth - _colourDepth);

            if (currentDiff < bestDiff)
            {
                bestDiff = currentDiff;
                bestIndex = j;
            }

            // We Love Intel's Compilers :)
            if ( currentDiff == 0 )
			{
                break; // No point continuing, direct match
			}
        }

        return &p[bestIndex];
    }
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge


