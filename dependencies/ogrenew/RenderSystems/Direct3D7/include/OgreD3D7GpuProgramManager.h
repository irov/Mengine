/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2005 The OGRE Team
Also see acknowledgements in Readme.html

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
-----------------------------------------------------------------------------
*/
#ifndef __D3D7GpuProgramManager_H__
#define __D3D7GpuProgramManager_H__

#include "OgreD3D7Prerequisites.h"
#include "OgreGpuProgramManager.h"

namespace Ogre {

    /** Dummy implementation of GpuProgram - cannot do anything
        since D3D7 did not support vertex or fragment programs. */
    class D3D7GpuProgram : public GpuProgram
    {
    public:
        D3D7GpuProgram(ResourceManager* creator, const String& name, ResourceHandle handle,
            const String& group, bool isManual = false, ManualResourceLoader* loader = 0)
            : GpuProgram(creator, name, handle, group, isManual, loader) {}

    protected:
        /** Overridden from GpuProgram, do nothing */
        void loadFromSource(void) {}
        /** Overridden from GpuProgram, do nothing */
        void unloadImpl(void) {}
    };
    /** Dummy implementation of GpuProgramManager - cannot do anything
        since D3D7 did not support vertex or fragment programs. */
    class D3D7GpuProgramManager : public GpuProgramManager
    {
    protected:
        Resource* createImpl(const String& name, ResourceHandle handle, 
            const String& group, bool isManual, ManualResourceLoader* loader,
            GpuProgramType gptype, const String& syntaxCode)
        {
            return new D3D7GpuProgram(this, name, handle, group, isManual, loader);
        }
        Resource* createImpl(const String& name, ResourceHandle handle, 
            const String& group, bool isManual, ManualResourceLoader* loader, 
            const NameValuePairList* createParams)
        {
            return new D3D7GpuProgram(this, name, handle, group, isManual, loader);
        }
    public:
        D3D7GpuProgramManager() 
        {
            // Register with resource group manager
            ResourceGroupManager::getSingleton()._registerResourceManager(mResourceType, this);
        }
		~D3D7GpuProgramManager() 
        {
            ResourceGroupManager::getSingleton()._unregisterResourceManager(mResourceType);
        }
        /// @copydoc GpuProgramManager::createParameters
        GpuProgramParametersSharedPtr createParameters(void) 
        { return GpuProgramParametersSharedPtr(new GpuProgramParameters()); }
    };

}

#endif
