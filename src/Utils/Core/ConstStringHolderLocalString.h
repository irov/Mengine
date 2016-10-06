#   pragma once

#	include "Core/ConstStringHolder.h"

namespace Menge
{
	class ConstStringHolderLocalString
		: public ConstStringHolder
	{	
	protected:
        void destroyString() override;
	};
}