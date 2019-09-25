#include "SpritePrototypeGenerator.h"

#include "Interface/EnumeratorServiceInterface.h"

#include "Kernel/Entity.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SpritePrototypeGenerator::SpritePrototypeGenerator( const SpriteCachePtr & _spriteCache )
        : m_spriteCache( _spriteCache )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SpritePrototypeGenerator::~SpritePrototypeGenerator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FactorablePointer SpritePrototypeGenerator::generate( const Char * _doc )
    {
        const FactoryPtr & factory = this->getFactory();

        SpritePtr sprite = factory->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( sprite, nullptr, "can't generate '%s' '%s' doc '%s'"
            , this->getCategory().c_str()
            , this->getPrototype().c_str()
            , _doc
        );

        const ConstString & prototype = this->getPrototype();
        sprite->setType( prototype );

        uint32_t uniqueIdentity = GENERATE_UNIQUE_IDENTITY();
        sprite->setUniqueIdentity( uniqueIdentity );

        this->setupScriptable( sprite );

        sprite->setSpriteCache( m_spriteCache );

        return sprite;
    }
}