#pragma once

#include "Interface/PrototypeServiceInterface.h"

#include "Kernel/DefaultPrototypeGenerator.h"
#include "Kernel/ObjectPrototypeGenerator.h"
#include "Kernel/IdentityPrototypeGenerator.h"
#include "Kernel/NodePrototypeGenerator.h"
#include "Kernel/ResourcePrototypeGenerator.h"
#include "Kernel/SurfacePrototypeGenerator.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        template<class T, uint32_t Count>
        bool addDefaultPrototype( const ConstString & _category, const DocumentInterfacePtr & _doc )
        {
            PrototypeGeneratorInterfacePtr generator = Helper::makeDefaultPrototypeGenerator<T, Count>( _doc );

            const ConstString & prototype = T::getFactorableType();

            bool result = PROTOTYPE_SERVICE()
                ->addPrototype( _category, prototype, generator );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T, uint32_t Count>
        bool addObjectPrototype( const ConstString & _category, const DocumentInterfacePtr & _doc )
        {
            PrototypeGeneratorInterfacePtr generator = Helper::makeObjectPrototypeGenerator<T, Count>( _doc );

            const ConstString & prototype = T::getFactorableType();

            bool result = PROTOTYPE_SERVICE()
                ->addPrototype( _category, prototype, generator );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T, uint32_t Count>
        bool addIdentityPrototype( const ConstString & _category, const DocumentInterfacePtr & _doc )
        {
            PrototypeGeneratorInterfacePtr generator = Helper::makeIdentityPrototypeGenerator<T, Count>( _doc );

            const ConstString & prototype = T::getFactorableType();

            bool result = PROTOTYPE_SERVICE()
                ->addPrototype( _category, prototype, generator );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T, uint32_t Count>
        bool addNodePrototype( const DocumentInterfacePtr & _doc )
        {
            PrototypeGeneratorInterfacePtr generator = Helper::makeNodePrototypeGenerator<T, Count>( _doc );

            const ConstString & category = Node::getFactorableType();
            const ConstString & prototype = T::getFactorableType();

            bool result = PROTOTYPE_SERVICE()
                ->addPrototype( category, prototype, generator );

            return result;            
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T, uint32_t Count>
        bool addResourcePrototype( const DocumentInterfacePtr & _doc )
        {
            PrototypeGeneratorInterfacePtr generator = Helper::makeResourcePrototypeGenerator<T, Count>( _doc );

            const ConstString & category = Resource::getFactorableType();
            const ConstString & prototype = T::getFactorableType();

            bool result = PROTOTYPE_SERVICE()
                ->addPrototype( category, prototype, generator );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T, uint32_t Count>
        bool addSurfacePrototype( const DocumentInterfacePtr & _doc )
        {
            PrototypeGeneratorInterfacePtr generator = Helper::makeSurfacePrototypeGenerator<T, Count>( _doc );

            const ConstString & category = Surface::getFactorableType();
            const ConstString & prototype = T::getFactorableType();

            bool result = PROTOTYPE_SERVICE()
                ->addPrototype( category, prototype, generator );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        void removePrototype( const ConstString & _category )
        {
            const ConstString & prototype = T::getFactorableType();

            PROTOTYPE_SERVICE()
                ->removePrototype( _category, prototype, nullptr );
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        void removeNodePrototype()
        {
            const ConstString & category = Node::getFactorableType();
            
            Helper::removePrototype<T>( category );
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        void removeResourcePrototype()
        {
            const ConstString & category = Resource::getFactorableType();
            
            Helper::removePrototype<T>( category );
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        void removeSurfacePrototype()
        {
            const ConstString & category = Surface::getFactorableType();
            
            Helper::removePrototype<T>( category );
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        void removeAffectorPrototype()
        {
            const ConstString & category = Affector::getFactorableType();

            Helper::removePrototype<T>( category );
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        FactorablePointer generatePrototype( const ConstString & _category, const DocumentInterfacePtr & _doc )
        {
            const ConstString & prototype = T::getFactorableType();

            FactorablePointer factorable = PROTOTYPE_SERVICE()
                ->generatePrototype( _category, prototype, _doc );

            return factorable;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        FactorablePointer generateFactorable( const DocumentInterfacePtr & _doc )
        {
            FactorablePointer factorable = Helper::generatePrototype<T>( ConstString::none(), _doc );

            return factorable;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        FactorablePointer generateNodeFactorable( const DocumentInterfacePtr & _doc )
        {
            const ConstString & category = Node::getFactorableType();

            FactorablePointer factorable = Helper::generatePrototype<T>( category, _doc );

            return factorable;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        FactorablePointer generateResourceFactorable( const DocumentInterfacePtr & _doc )
        {
            const ConstString & category = Resource::getFactorableType();

            FactorablePointer factorable = Helper::generatePrototype<T>( category, _doc );

            return factorable;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        FactorablePointer generateSurfaceFactorable( const DocumentInterfacePtr & _doc )
        {
            const ConstString & category = Surface::getFactorableType();

            FactorablePointer factorable = Helper::generatePrototype<T>( category, _doc );

            return factorable;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        FactorablePointer generateAffectorFactorable( const DocumentInterfacePtr & _doc )
        {
            const ConstString & category = Affector::getFactorableType();

            FactorablePointer factorable = Helper::generatePrototype<T>( category, _doc );

            return factorable;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}