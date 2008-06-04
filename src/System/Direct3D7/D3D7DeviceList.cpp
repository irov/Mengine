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

        deviceList->AddDevice(lpDeviceDescription, lpDeviceName,
                                lpD3DDeviceDesc);

        // Continue enumeration
        return D3DENUMRET_OK;
    }
	//////////////////////////////////////////////////////////////////////////
    D3DDeviceList::D3DDeviceList(LPDIRECT3D7 direct3D)
    {
        // Will create a  new driver list and enumerate it
        if (direct3D == NULL)
            throw Exception(Exception::ERR_INVALIDPARAMS, "NULL has been incorrectly passed as a "
                "D3D interface pointer.", "D3DDeviceList Contructor");

        lpD3D = direct3D;
        // Enumerate the list
        enumerate();

    }
	//////////////////////////////////////////////////////////////////////////
    D3DDeviceList::~D3DDeviceList()
    {
		for(size_t i=0; i<count(); i++)
		{
			item(i)->cleanup();
		}
        mDeviceList.clear();

    }
	//////////////////////////////////////////////////////////////////////////
    BOOL D3DDeviceList::enumerate()
    {
        HRESULT hr;

        LogManager::getSingleton().logMessage("----- Direct3D Detection Starts");

        hr = lpD3D->EnumDevices(D3DEnumDevicesCallback, this);
        if (FAILED(hr))
            throw Exception(Exception::ERR_RENDERINGAPI_ERROR, "Error enumerating 3D devices", "D3DDeviceList - enumerate");

        LogManager::getSingleton().logMessage("----- Direct3D Detection Ends");

        return TRUE;
    }
	//////////////////////////////////////////////////////////////////////////
    void D3DDeviceList::AddDevice(LPSTR lpDeviceDesc,
                                       LPSTR lpDeviceName,
                                       LPD3DDEVICEDESC7 lpD3DDeviceDesc)
    {
        D3DDevice *newD3D;

        // Check to see if this is a duff driver
        // Handle specific device GUIDs. NullDevice renders nothing
        if (IsEqualGUID(lpD3DDeviceDesc->deviceGUID, IID_IDirect3DNullDevice))
            return;


        // Create new driver
        newD3D = new D3DDevice(lpD3D, lpDeviceDesc, lpDeviceName, lpD3DDeviceDesc);

        // Add it to my list
        mDeviceList.push_back(*newD3D);

        delete newD3D;
    }
	//////////////////////////////////////////////////////////////////////////
    size_t D3DDeviceList::count(void)
    {
        return mDeviceList.size();
    }
	//////////////////////////////////////////////////////////////////////////
    D3DDevice* D3DDeviceList::item(size_t index)
    {
		return &mDeviceList[index];
    }
	//////////////////////////////////////////////////////////////////////////
    D3DDevice* D3DDeviceList::getBest(unsigned int minColourDepth)
    {

        std::vector<D3DDevice>::iterator p = mDeviceList.begin();
        std::vector<D3DDevice>::iterator bestDevice = mDeviceList.end();
        static D3DDevice* savedBest = 0;

        if (savedBest)
            return savedBest;
        LogManager::getSingleton().logMessage("Determining best 3D Device...");

        // For now, just get ANY hardware device that can match the following
        // minimum requirements
        // 2. Colour depth = primary surface colour depth
        // Add preference to TnL devices
        while (p != mDeviceList.end())
        {
            if (p->HardwareAccelerated())
            {
                // Check minimum render depth
                if ( (p->RenderBitDepth() >= minColourDepth))
                {
                    // Ok, minimum caps have been satisfied so we can consider using HW
                    // Any device yet?
                    if (bestDevice == mDeviceList.end())
                        bestDevice = p;
                    // Always override SW device
                    else if (!bestDevice->HardwareAccelerated())
                        bestDevice = p;
                    // Add preference to HW TnL
                    else if (p->CanHWTransformAndLight())
                        bestDevice = p;

                }
            }
            else
            {
                // Software device, save for fallback
                if (bestDevice == mDeviceList.end())
                    bestDevice = p;
            }

            p++;

        }

        LogManager::getSingleton().logMessage("Best 3D Device is: " + bestDevice->DeviceDescription());

        savedBest = &(*bestDevice);
        return savedBest;

    }
	//////////////////////////////////////////////////////////////////////////
}	// Namespace Menge
