#	pragma once

#	include "Config/Typedef.h"

#	include "Core/Holder.h"
#	include "Core/ConstString.h"

#	include <map>

namespace Menge
{
	class Arrow;

	struct ArrowDesc
	{
		ConstString pak;
		ConstString path;
	};

	class ArrowManager
		: public Holder<ArrowManager>
	{
	public:
		ArrowManager();
		~ArrowManager();

	public:
		void registerArrow( const ConstString & _name, const ArrowDesc & _desc );

		Arrow * createArrow( const ConstString & _name, const ConstString & _prototype );
		void removeArrow( const ConstString& _name );

	protected:
		Arrow * createArrow_( const ConstString & _name, const ConstString & _prototype );

	protected:
		typedef std::map<ConstString, Arrow *> TMapArrows;
		TMapArrows m_arrows;

		typedef std::map<ConstString, ArrowDesc> TMapDescriptionArrows;
		TMapDescriptionArrows m_descriptions;
	};
}