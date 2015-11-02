#	pragma once

#   include "Interface/PrototypeManagerInterface.h"

#	include "Core/ConstString.h"

#   include "stdex/stl_map.h"

namespace Menge
{
	class PrototypeManager
        : public ServiceBase<PrototypeServiceInterface>
	{
	public:
		PrototypeManager();
		~PrototypeManager();

	public:
		bool _initialize() override;
		void _finalize() override;

	public:
		bool addPrototype( const ConstString & _category, const ConstString & _prototype, const PrototypeGeneratorInterfacePtr & _generator ) override;
		bool hasPrototype( const ConstString & _category, const ConstString & _prototype, PrototypeGeneratorInterfacePtr & _generator ) const override;

    public:
        Factorable * generatePrototype( const ConstString & _category, const ConstString & _prototype ) override;

    public:
        void visitGenerators( VisitorPrototypeGenerator * _visitor ) const override;

	protected:
        struct CategoryKey
        {
            ConstString category;
            ConstString prototype;
        };

        struct CategoryKeyLess
        {
            bool operator () ( const CategoryKey & _left, const CategoryKey & _right ) const
            {
                if( _left.category < _right.category )
                {
                    return true;
                }
                else if( _left.category == _right.category )
                {
                    return _left.prototype < _right.prototype;
                }

                return false;
            }
        };

        typedef stdex::map<CategoryKey, PrototypeGeneratorInterfacePtr, CategoryKeyLess> TMapPrototypes;
		TMapPrototypes m_prototypes;       
	};
}