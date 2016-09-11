#	pragma once

#	include "Interface/COMInterface.h"

#	include "Factory/FactorablePtr.h"

#	include "Kernel/Identity.h"
#	include "Kernel/Updatable.h"
#	include "Kernel/Materialable.h"
#	include "Kernel/Eventable.h"
#	include "Kernel/Scriptable.h"
#	include "Kernel/Animatable.h"
#	include "Kernel/Comable.h"
#	include "Kernel/Servant.h"

namespace Menge
{
	class Surface
		: public FactorablePtr
		, public Identity
		, public Updatable
		, public Materialable
		, public Eventable
		, public Scriptable
		, public Animatable
		, public Comable
		, public Servant
	{
	public:
		Surface();
		~Surface();
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<Surface> SurfacePtr;
}