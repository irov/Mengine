#	pragma once

#	include "Loadable.h"

#	include "Resource.h"
#	include "Reference.h"

#	include "ResourceDeclare.h"

#	include "Factory/Factorable.h"
#	include "Factory/Factory.h"

namespace Menge
{
	class ResourceVisitor;
	class FactoryIdentity;
	
	struct ResourceFactoryParam
	{
		String name;
		String category;
		String group;
		String file;
	};

	struct ResourceFactoryIdentity
	{
		std::size_t name;
		std::size_t category;
		std::size_t group;
		std::size_t file;
	};

	class ResourceReference
		: public Factorable
		, public Resource
		, public Reference
		, public Loadable
	{
	public:
		ResourceReference();
		~ResourceReference();

	public:
		void initialize( const ResourceFactoryParam & _params, FactoryIdentity * _factoryIdentity );
		void initialize( const ResourceFactoryIdentity & _params, FactoryIdentity * _factoryIdentity );

	public:
		const String & getName() const;
		const String & getCategory() const;
		const String & getGroup() const;
		const String & getFile() const;

	public:
		std::size_t getNameIdentity() const;
		std::size_t getCategoryIdentity() const;
		std::size_t getGroupIdentity() const;
		std::size_t getFileIdentity() const;

	public:
		void loader( XmlElement * _xml ) override;
		void parser( BinParser * _parser ) override;

	public:
		virtual void accept( ResourceVisitor * _visitor ) = 0;

	protected:
		bool _incrementZero() override;
		void _decrementZero() override;

	protected:
		FactoryIdentity * m_factoryIdentity;

		std::size_t m_nameIdentity;
		std::size_t m_categoryIdentity;
		std::size_t m_groupIdentity;
		std::size_t m_fileIdentity;
	};
}
