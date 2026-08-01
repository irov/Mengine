#pragma once

#include "../MCPFileGroup.h"

#include "Interface/ContentInterface.h"

#include "Kernel/Map.h"

#include "jpp/jpp.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MCPResourceContext
    {
    public:
        MCPFileGroupPtr getOverlayFileGroup( const FileGroupInterfacePtr & _baseFileGroup );
        MCPFileGroupPtr findOverlayFileGroup( const FileGroupInterfacePtr & _baseFileGroup ) const;
        bool recompileOverlayResources( const ConstString & _fileGroup, const FilePath & _filePath, const MCPFileGroupPtr & _overlay, jpp::array * const _resources, String * const _error );
        void revertAllOverlays();
        size_t getOverlayGroupCount() const;

    protected:
        typedef Map<FileGroupInterface *, MCPFileGroupPtr> MapOverlayGroups;
        typedef Map<ContentInterfacePtr, FileGroupInterfacePtr> MapOriginalGroups;

        MapOverlayGroups m_overlayGroups;
        MapOriginalGroups m_overlayOriginalGroups;
    };
    //////////////////////////////////////////////////////////////////////////
}
