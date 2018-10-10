#include "ParticleResourceValidateVisitor.h"

#include "Interface/ResourceServiceInterface.h"
#include "Interface/RenderTextureInterface.h"
#include "Interface/RenderImageInterface.h"
#include "Interface/FileSystemInterface.h"
#include "Interface/ConfigInterface.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ParticleResourceValidateVisitor::ParticleResourceValidateVisitor()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ParticleResourceValidateVisitor::~ParticleResourceValidateVisitor()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ParticleResourceValidateVisitor::accept( ResourceParticle * _resource )
    {
        const FilePath & filePath = _resource->getFilePath();
        const ConstString & name = _resource->getName();
        
        const FileGroupInterfacePtr & fileGroup = _resource->getFileGroup();

        ParticleEmitterContainerInterface2Ptr container = PARTICLE_SERVICE2()
            ->createEmitterContainerFromFile( fileGroup, filePath, name );

        if( container == nullptr )
        {
            LOGGER_ERROR( "ResourceParticle::_isValid '%s' group '%s' can't create container file '%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , _resource->getFilePath().c_str()
            );

            return false;
        }

        if( container->isValid() == false )
        {
            LOGGER_ERROR( "ResourceParticle::_isValid %s can't valid container '%s'"
                , _resource->getName().c_str()
                , _resource->getFilePath().c_str()
            );

            return false;
        }

        return true;
    }    
}