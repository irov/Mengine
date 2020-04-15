#pragma once

#include "Interface/VocabularyServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/ConstString.h"
#include "Kernel/Hashtable2.h"

#ifndef MENGINE_MASTER_RELEASE
#   include "Kernel/Map.h"
#endif

namespace Mengine
{
    class VocabularyService
        : public ServiceBase<VocabularyServiceInterface>
    {
    public:
        VocabularyService();
        ~VocabularyService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        bool setFactorable( const ConstString & _category, const ConstString & _type, const MixinPtr & _factorable ) override;
        MixinPointer removeFactorable( const ConstString & _category, const ConstString & _type ) override;
        MixinPointer getFactorable( const ConstString & _category, const ConstString & _type ) const override;
        bool hasFactorable( const ConstString & _category, const ConstString & _type ) const override;

    protected:
        void foreachVocabulary( const ConstString & _category, const LambdaVocabulary & _lambda ) const override;

    protected:
        typedef Hashtable2<ConstString, ConstString, MixinPtr> HashtableMixins;
        HashtableMixins m_mixins;
    };
}