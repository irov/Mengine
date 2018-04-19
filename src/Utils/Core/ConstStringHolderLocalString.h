#pragma once

#include "Core/ConstStringHolder.h"

namespace Mengine
{
	class ConstStringHolderLocalString
		: public ConstStringHolder
	{	
	};
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<ConstStringHolderLocalString> ConstStringHolderLocalStringPtr;
}