#include "MCPResourceReloadCommand.h"

#include "../../Contexts/MCPResourceContext.h"

#include "Interface/FileServiceInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Vector.h"

#include "Config/StdString.h"
#include "Config/StdUtility.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPResourceReloadCommand::MCPResourceReloadCommand( MCPResourceContext * _context )
        : m_context( _context )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPResourceReloadCommand::execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        jpp::object entriesObject;

        if( _request.params.exist( "entries", &entriesObject ) == false || entriesObject.is_type_array() == false )
        {
            _response->errorMessage = "entries must be an array";

            return EMCPCommandStatus::FAILURE;
        }

        struct ReloadEntry
        {
            ConstString group;
            FilePath path;
            Data data;
            Data previous;
            MCPFileGroupPtr overlay;
            bool hadPrevious;
            bool unchanged;
            bool structural;
            bool restartRequired;
        };

        static const uint32_t invalidRange = 0xFFFFFFFFU;
        Vector<ReloadEntry> entries;
        jpp::array entryValues( entriesObject );

        for( const jpp::object & value : entryValues )
        {
            const Char * groupName = value.get( "fileGroup", "" );
            const Char * logicalPath = value.get( "logicalPath", "" );
            uint32_t offset = value.get( "offset", invalidRange );
            uint32_t size = value.get( "size", invalidRange );
            size_t attachmentSize = _request.attachment.size();

            if( logicalPath[0] == '\0' || offset == invalidRange || size == invalidRange || offset > attachmentSize || size > attachmentSize - offset )
            {
                _response->errorMessage = "resource entry has invalid path, offset, or size";

                return EMCPCommandStatus::FAILURE;
            }

            ConstString group = Helper::stringizeString( groupName );
            FileGroupInterfacePtr baseFileGroup;
            bool hasFileGroup = FILE_SERVICE()->hasFileGroup( group, &baseFileGroup );

            if( hasFileGroup == false )
            {
                _response->errorMessage = "resource file group is not mounted";

                return EMCPCommandStatus::FAILURE;
            }

            ReloadEntry entry;
            entry.group = group;
            entry.path = Helper::stringizeFilePath( logicalPath );
            const uint8_t * attachmentData = _request.attachment.data();
            entry.data.assign( attachmentData + offset, attachmentData + offset + size );
            entry.overlay = m_context->getOverlayFileGroup( baseFileGroup );

            if( entry.overlay == nullptr )
            {
                _response->errorMessage = "failed to create resource overlay file group";

                return EMCPCommandStatus::FAILURE;
            }

            entry.hadPrevious = entry.overlay->getOverlay( entry.path, &entry.previous );
            entry.unchanged = entry.hadPrevious == true && entry.previous == entry.data;
            entry.structural = StdString::strstr( logicalPath, "Package.json" ) != nullptr
                || StdString::strstr( logicalPath, "Packages.json" ) != nullptr
                || StdString::strstr( logicalPath, "Pak.xml" ) != nullptr;
            entry.restartRequired = entry.structural;
            entries.emplace_back( StdUtility::move( entry ) );
        }

        for( ReloadEntry & entry : entries )
        {
            if( entry.structural == false && entry.unchanged == false )
            {
                entry.overlay->setOverlay( entry.path, entry.data );
            }
        }

        bool rollback = false;
        Vector<jpp::array> affected;
        size_t entryCount = entries.size();
        affected.reserve( entryCount );

        for( ReloadEntry & entry : entries )
        {
            jpp::array resources = jpp::make_array();
            String reloadError;

            if( entry.structural == false && entry.unchanged == false )
            {
                bool recompiled = m_context->recompileOverlayResources( entry.group, entry.path, entry.overlay, &resources, &reloadError );

                if( recompiled == false )
                {
                    const Char * reloadErrorString = reloadError.c_str();
                    entry.restartRequired = StdString::strstr( reloadErrorString, "restart" ) != nullptr;
                    rollback = true;
                }
            }

            affected.emplace_back( resources );
        }

        if( rollback == true )
        {
            for( ReloadEntry & entry : entries )
            {
                if( entry.structural == true || entry.unchanged == true )
                {
                    continue;
                }

                if( entry.hadPrevious == true )
                {
                    entry.overlay->setOverlay( entry.path, entry.previous );
                }
                else
                {
                    entry.overlay->removeOverlay( entry.path );
                }

                jpp::array restored = jpp::make_array();
                String restoreError;
                bool restoredSuccessful = m_context->recompileOverlayResources( entry.group, entry.path, entry.overlay, &restored, &restoreError );

                if( restoredSuccessful == false )
                {
                    const Char * group = entry.group.c_str();
                    const Char * path = entry.path.c_str();
                    const Char * error = restoreError.c_str();

                    LOGGER_ERROR( "MCP failed to restore last-good resource '%s:%s': %s"
                        , group
                        , path
                        , error
                    );
                }
            }
        }

        jpp::array results = jpp::make_array();

        for( size_t index = 0; index != entries.size(); ++index )
        {
            const ReloadEntry & entry = entries[index];
            jpp::object item = jpp::make_object();
            item.set( "fileGroup", entry.group );
            item.set( "logicalPath", entry.path );
            item.set( "resources", affected[index] );

            if( entry.restartRequired == true )
            {
                item.set( "status", "restart_required" );
            }
            else if( entry.unchanged == true )
            {
                item.set( "status", "unchanged" );
            }
            else if( rollback == true )
            {
                item.set( "status", "failed" );
            }
            else
            {
                item.set( "status", "reloaded" );
            }

            results.push_back( item );
        }

        _response->result.set( "results", results );
        _response->result.set( "rolledBack", rollback );

        return EMCPCommandStatus::SUCCESS;
    }
    //////////////////////////////////////////////////////////////////////////
}
