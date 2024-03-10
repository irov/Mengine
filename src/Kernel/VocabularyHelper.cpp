#include "VocabularyHelper.h"

#include "Interface/VocabularyServiceInterface.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void vocabularySet( const ConstString & _category, const ConstString & _key, const MixinPtr & _value, const DocumentInterfacePtr & _doc )
        {
            VOCABULARY_SERVICE()
                ->setFactorable( _category, _key, _value, _doc );
        }
        //////////////////////////////////////////////////////////////////////////
        MixinPointer vocabularyRemove( const ConstString & _category, const ConstString & _prototype )
        {
            MixinPointer value = VOCABULARY_SERVICE()
                ->removeFactorable( _category, _prototype );

            return value;

        }
        //////////////////////////////////////////////////////////////////////////
        bool vocabularyHas( const ConstString & _category, const ConstString & _prototype )
        {
            bool result = VOCABULARY_SERVICE()
                ->hasFactorable( _category, _prototype );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        MixinPointer vocabularyGet( const ConstString & _category, const ConstString & _prototype )
        {
            MixinPointer value = VOCABULARY_SERVICE()
                ->getFactorable( _category, _prototype );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}