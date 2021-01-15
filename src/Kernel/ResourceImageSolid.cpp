#include "ResourceImageSolid.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceImageSolid::ResourceImageSolid()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceImageSolid::~ResourceImageSolid()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceImageSolid::_compile()
    {
        if( ResourceImage::_compile() == false )
        {
            LOGGER_ERROR( "'%s' invalid compile base ResourceImage class"
                , this->getName().c_str()
            );

            return false;
        }

        this->setPow2( true );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}