#	pragma once

#include "D3D7RenderSystem.h"
#include "D3D7DeviceList.h"
#include "D3D7Device.h"
#include "DDDriver.h"
//#include "OgreLogManager.h"
//#include "OgreException.h"

namespace Menge
{
    // Non-member callback functions
    static HRESULT CALLBACK D3DEnumDevicesCallback(
                                    LPSTR lpDeviceDescription,
                                    LPSTR lpDeviceName,
                                    LPD3DDEVICEDESC7 lpD3DDeviceDesc,
                                    LPVOID lpContext)
    {
        D3DDeviceList* deviceList;

        deviceList = (D3DDeviceList*) lpContext;

        deviceList->addDevice_(lpDeviceDescription, lpDeviceName,
                                lpD3DDeviceDesc);

        // Continue enumeration
        return D3DENUMRET_OK;
    }
	//////////////////////////////////////////////////////////////////////////
    D3DDeviceList::D3DDeviceList(LPDIRECT3D7 direct3D)
    {
        // Will create a  new driver list and enumerate it
        assert( direct3D && "D3DDeviceList Contructor -> NULL has been incorrectly passed as a "
                "D3D interface pointer." );

        lpD3D = direct3D;
        // Enumerate the list
        enumerate();

    }
	//////////////////////////////////////////////////////////////////////////
    D3DDeviceList::~D3DDeviceList()
    {
		for( std::size_t i=0; i < count(); i++ )
		{
			item(i)->cleanup();
		}
        m_deviceList.clear();
    }
	//////////////////////////////////////////////////////////////////////////
    BOOL D3DDeviceList::enumerate()
    {
        HRESULT hr;

        m_logManager->logMessage("----- Direct3D Detection Starts");

        hr = lpD3D->EnumDevices(D3DEnumDevicesCallback, this);
        assert( SUCCEEDED(hr) && "D3DDeviceList - enumerate -> Error enumerating 3D devices" );

        m_logManager->logMessage("----- Direct3D Detection Ends");

        return TRUE;
    }
	//////////////////////////////////////////////////////////////////////////
    void D3DDeviceList::addDevice_(LPSTR lpDeviceDesc,
                                       LPSTR lpDeviceName,
                                       LPD3DDEVICEDESC7 lpD3DDeviceDesc)
    {
        D3DDevice *newD3D;

        // Check to see if this is a duff driver
        // Handle specific device GUIDs. NullDevice renders nothing
        if (IsEqualGUID(lpD3DDeviceDesc->deviceGUID, IID_IDirect3DNullDevice))
            return;


        // Create new driver
        newD3D = new D3DDevice( lpD3D, lpDeviceDesc, lpDeviceName, lpD3DDeviceDesc );
		newD3D->initialize( m_logManager );

        // Add it to my list
        m_deviceList.push_back(*newD3D);

        delete newD3D;
    }
	//////////////////////////////////////////////////////////////////////////
	std::size_t D3DDeviceList::count()
    {
        return m_deviceList.size();
    }
	//////////////////////////////////////////////////////////////////////////
	D3DDevice* D3DDeviceList::item( std::size_t _index )
    {
		return &m_deviceList[_index];
    }
	//////////////////////////////////////////////////////////////////////////
    D3DDevice* D3DDeviceList::getBest( unsigned int _minColourDepth )
    {

        std::vector<D3DDevice>::iterator p = m_deviceList.begin();
        std::vector<D3DDevice>::iterator bestDevice = m_deviceList.end();
        static D3DDevice* savedBest = 0;

        if( savedBest )
		{
            return savedBest;
		}
        m_logManager->logMessage("Determining best 3D Device...");

        // For now, just get ANY hardware device that can match the following
        // minimum requirements
        // 2. Colour depth = primary surface colour depth
        // Add preference to TnL devices
        while (p != m_deviceList.end())
        {
            if (p->isHardwareAccelerated())
            {
                // Check minimum render depth
                if ( (p->renderBitDepth() >= _minColourDepth))
                {
                    // Ok, minimum caps have been satisfied so we can consider using HW
                    // Any device yet?
                    if (bestDevice == m_deviceList.end())
					{
                        bestDevice = p;
					}
                    // Always override SW device
                    else if (!bestDevice->isHardwareAccelerated())
					{
                        bestDevice = p;
					}
                    // Add preference to HW TnL
                    else if (p->canHWTransformAndLight())
					{
                        bestDevice = p;
					}

                }
            }
            else
            {
                // Software device, save for fallback
                if (bestDevice == m_deviceList.end())
				{
                    bestDevice = p;
				}
            }

            p++;

        }

        m_logManager->logMessage("Best 3D Device is: " + bestDevice->getDeviceDescription() );

        savedBest = &(*bestDevice);
        return savedBest;

    }
	//////////////////////////////////////////////////////////////////////////
}	// Namespace Menge
