#	pragma once

#	include "Manager/ConstManager.h"

#	include <map>

namespace Menge
{
	class Arrow;

	struct ArrowDesc
	{
		ConstString pak;
		String path;
	};

	class ArrowManager
		: public Holder<ArrowManager>
	{
	public:
		ArrowManager();
		~ArrowManager();

	public:
		void registerArrow( const ConstString & _name, const ArrowDesc & _desc );

		Arrow * getArrow( const ConstString & _name );
		void removeArrow( const ConstString& _name );

	protected:
		Arrow * createArrow_( const ConstString & _name );

	protected:
		typedef std::map<ConstString, Arrow *> TMapArrows;
		TMapArrows m_arrows;

		typedef std::map<ConstString, ArrowDesc> TMapDescriptionArrows;
		TMapDescriptionArrows m_descriptions;
	};
}