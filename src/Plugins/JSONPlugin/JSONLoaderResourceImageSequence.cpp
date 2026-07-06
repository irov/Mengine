#include "JSONLoaderResourceImageSequence.h"

#include "Interface/ResourceBankInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionResourceType.h"
#include "Kernel/JSONDataHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceImageSequence::JSONLoaderResourceImageSequence()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceImageSequence::~JSONLoaderResourceImageSequence()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONLoaderResourceImageSequence::load( const FactorablePtr & _factorable, const jpp::object & _json )
    {
        ResourceImageSequence * resource = _factorable.getT<ResourceImageSequence *>();

        const ConstString & groupName = resource->getGroupName();

        ResourceBankInterface * resourceBank = resource->getResourceBank();

        jpp::object j_sequences;
        if( _json.exist( "Sequence", &j_sequences ) == false )
        {
            resource->setSequenceDuration( 0.f );

            return true;
        }

        if( j_sequences.is_type_array() == false )
        {
            return false;
        }

        float duration = 0.f;

        for( const jpp::object & meta_sequence : jpp::array( j_sequences ) )
        {
            ConstString resourceName;
            float delay;

            if( Helper::getJSONConstString( meta_sequence, "ResourceImageName", &resourceName ) == false ||
                Helper::getJSONFloat( meta_sequence, "Delay", &delay ) == false )
            {
                return false;
            }

            duration += delay;

            MENGINE_ASSERTION_RESOURCE_TYPE_BY_NAME( resourceName, ResourceImagePtr, false, "resource '%s' type does not match 'ResourceImage'"
                , resourceName.c_str()
            );

            const ResourceImagePtr & resourceImage = resourceBank->getResource( groupName, resourceName );

            MENGINE_ASSERTION_MEMORY_PANIC( resourceImage, "'%s' group '%s' invalid get sequence image resource '%s'"
                , resource->getName().c_str()
                , resource->getGroupName().c_str()
                , resourceName.c_str()
            );

            resource->addFrame( resourceImage, delay );
        }

        resource->setSequenceDuration( duration );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
