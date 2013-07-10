#	pragma once

#   include "Interface/PrototypeManagerInterface.h"

#	include "Core/ConstString.h"

#   include "stdex/binary_vector.h"

namespace Menge
{
	class PrototypeManager
        : public PrototypeServiceInterface
	{
	public:
		PrototypeManager();
		~PrototypeManager();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

	public:
		bool addPrototype( const ConstString & _category, const ConstString & _prototype, PrototypeGeneratorInterface * _generator ) override;
		bool hasPrototype( const ConstString & _category, const ConstString & _prototype, PrototypeGeneratorInterface ** _generator ) const override;

    public:
        Factorable * generatePrototype( const ConstString & _category, const ConstString & _prototype ) override;

    public:
        void visitGenerators( VisitorPrototypeGenerator * _visitor ) const override;

	protected:
        ServiceProviderInterface * m_serviceProvider;

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

        typedef stdex::binary_vector<CategoryKey, PrototypeGeneratorInterface *, CategoryKeyLess> TMapPrototypes;        
		TMapPrototypes m_prototypes;       
	};
}