#include "MCPResourceRevertCommand.h"

#include "../../Contexts/MCPResourceContext.h"

#include "Interface/FileServiceInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPResourceRevertCommand::MCPResourceRevertCommand( MCPResourceContext * _context )
        : m_context( _context )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPResourceRevertCommand::execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        jpp::object resourcesObject;

        if( _request.params.exist( "resources", &resourcesObject ) == false || resourcesObject.is_type_array() == false )
        {
            _response->errorMessage = "resources must be an array";

            return EMCPCommandStatus::FAILURE;
        }

        jpp::array results = jpp::make_array();
        jpp::array resources( resourcesObject );

        for( const jpp::object & value : resources )
        {
            const Char * fileGroupValue = value.get( "fileGroup", "" );
            const Char * logicalPathValue = value.get( "logicalPath", "" );
            ConstString group = Helper::stringizeString( fileGroupValue );
            FilePath path = Helper::stringizeFilePath( logicalPathValue );
            FileGroupInterfacePtr baseFileGroup;
            jpp::object item = jpp::make_object();
            item.set( "fileGroup", group );
            item.set( "logicalPath", path );
            bool hasFileGroup = FILE_SERVICE()->hasFileGroup( group, &baseFileGroup );

            if( hasFileGroup == false )
            {
                item.set( "status", "unsupported" );
                results.push_back( item );

                continue;
            }

            MCPFileGroupPtr overlay = m_context->findOverlayFileGroup( baseFileGroup );

            if( overlay == nullptr || overlay->hasOverlay( path ) == false )
            {
                item.set( "status", "unchanged" );
                results.push_back( item );

                continue;
            }

            overlay->removeOverlay( path );
            jpp::array affected = jpp::make_array();
            String error;
            bool recompiled = m_context->recompileOverlayResources( group, path, overlay, &affected, &error );

            if( recompiled == false )
            {
                item.set( "status", "failed" );
            }
            else
            {
                item.set( "status", "reloaded" );
            }

            item.set( "resources", affected );
            results.push_back( item );
        }

        _response->result.set( "results", results );

        return EMCPCommandStatus::SUCCESS;
    }
    //////////////////////////////////////////////////////////////////////////
}
