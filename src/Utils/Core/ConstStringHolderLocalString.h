#pragma once

#include "Core/ConstStringHolder.h"

namespace Mengine
{
	class ConstStringHolderLocalString
		: public ConstStringHolder
	{	
	protected:
        void destroyString() override;
	};
}