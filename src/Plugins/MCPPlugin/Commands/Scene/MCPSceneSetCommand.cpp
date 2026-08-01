#include "MCPSceneSetCommand.h"

#include "../../Contexts/MCPSceneContext.h"

#include "Interface/TransformationInterface.h"
#include "Interface/RenderInterface.h"

#include "Kernel/ConstStringHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static bool readMCPVec3JSON( const jpp::object & _value, mt::vec3f * const _result )
        {
            if( _value.is_type_array() == false )
            {
                return false;
            }

            jpp::array array( _value );

            if( array.size() < 2 )
            {
                return false;
            }

            _result->x = array[0];
            _result->y = array[1];
            _result->z = array.size() >= 3 ? (float)array[2] : 0.f;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    MCPSceneSetCommand::MCPSceneSetCommand( MCPSceneContext * _context )
        : m_context( _context )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPSceneSetCommand::execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        m_context->refreshGeneration();

        const Char * handle = _request.params.get( "handle", "" );
        String error;
        NodePtr node = m_context->resolveNode( handle, &error );

        if( node == nullptr )
        {
            _response->errorCode = "stale_handle";
            _response->errorMessage = error;

            return EMCPCommandStatus::FAILURE;
        }

        jpp::object properties;

        if( _request.params.exist( "properties", &properties ) == false || properties.is_type_object() == false )
        {
            _response->errorMessage = "properties must be an object";

            return EMCPCommandStatus::FAILURE;
        }

        jpp::object property;

        if( properties.exist( "name", &property ) == true )
        {
            const Char * name = property;
            ConstString nodeName = Helper::stringizeString( name );
            node->setName( nodeName );
        }

        if( properties.exist( "enabled", &property ) == true )
        {
            bool enabled = property;

            if( enabled == true )
            {
                node->enable();
            }
            else
            {
                node->disable();
            }
        }

        TransformationInterface * transformation = node->getTransformation();

        if( transformation != nullptr )
        {
            mt::vec3f value;

            if( properties.exist( "position", &property ) == true && Detail::readMCPVec3JSON( property, &value ) == true )
            {
                transformation->setLocalPosition( value );
            }

            if( properties.exist( "scale", &property ) == true && Detail::readMCPVec3JSON( property, &value ) == true )
            {
                transformation->setLocalScale( value );
            }

            if( properties.exist( "orientation", &property ) == true && Detail::readMCPVec3JSON( property, &value ) == true )
            {
                transformation->setLocalOrientation( value );
            }
        }

        RenderInterface * render = node->getRender();

        if( render != nullptr )
        {
            if( properties.exist( "visible", &property ) == true )
            {
                bool visible = property;
                render->setHide( visible == false );
            }

            if( properties.exist( "zGroup", &property ) == true )
            {
                ZGroupType zGroup = property;
                render->setZGroup( zGroup );
            }

            if( properties.exist( "zIndex", &property ) == true )
            {
                ZIndexType zIndex = property;
                render->setZIndex( zIndex );
            }

            if( properties.exist( "color", &property ) == true && property.is_type_array() == true )
            {
                jpp::array color( property );

                if( color.size() >= 3 )
                {
                    float red = color[0];
                    float green = color[1];
                    float blue = color[2];
                    float alpha = color.size() >= 4 ? (float)color[3] : 1.f;
                    render->setLocalColorRGBA( red, green, blue, alpha );
                }
            }
        }

        jpp::object result = m_context->makeNodePropertiesJSON( node );
        _response->result = result;

        return EMCPCommandStatus::SUCCESS;
    }
    //////////////////////////////////////////////////////////////////////////
}
