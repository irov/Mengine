#pragma once

#include "Kernel/ScriptablePrototypeGenerator.h"

#include "SpriteCache.h"
#include "Sprite.h"

namespace Mengine
{
    class SpritePrototypeGenerator
        : public ScriptablePrototypeGenerator<Sprite, 128>
    {
    public:
        SpritePrototypeGenerator( const SpriteCachePtr & _spriteCache );
        ~SpritePrototypeGenerator() override;

    protected:
        FactorablePointer generate( const Char * _doc ) override;

    protected:
        SpriteCachePtr m_spriteCache;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SpritePrototypeGenerator, PrototypeGeneratorInterface> SpritePrototypeGeneratorPtr;
    //////////////////////////////////////////////////////////////////////////
}