#	pragma once

#	include "Config/Typedef.h"

#	include "Core/Holder.h"
#	include "Core/ConstString.h"

#	include "ResourceManager.h"

#	include <map>

namespace Menge
{
	class Arrow;

	class ArrowManager
		: public Holder<ArrowManager>
	{
	public:
		ArrowManager();
		~ArrowManager();

	public:
		void registerArrow( const ResourceDesc & _desc );


	public:
		Arrow * createArrow( const ConstString & _name, const ConstString & _prototype );

	protected:
		Arrow * createArrow_( const ConstString & _name, const ConstString & _prototype );

	protected:
		typedef std::map<ConstString, ResourceDesc> TMapDescriptionArrows;
		TMapDescriptionArrows m_descriptions;
	};
}