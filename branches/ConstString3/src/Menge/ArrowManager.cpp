#	include "ArrowManager.h"
#	include "Arrow.h"

#	include <memory>

#	include "ScriptEngine.h"

#	include "Consts.h"
#	include "LogEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Arrow * ArrowManager::createArrow( const ConstString & _prototype )
	{
		PyObject * py_prototype = this->getPrototype( _prototype );

		if( py_prototype == 0 )
		{
			MENGE_LOG_ERROR( "ArrowManager.createArrow: arrow prototype '%s' not found"
				, _prototype.c_str()
				);

			return 0;
		}

		const ConstString & type = Consts::get()->c_Arrow;

		Arrow * arrow = ScriptEngine::get()
			->createEntityT<Arrow>( type, py_prototype );

		if( arrow == 0 )
		{
			MENGE_LOG_ERROR( "ArrowManager.createArrow: can't create arrow from prototype '%s'"
				, _prototype.c_str()
				); 

			return 0;
		}

		return arrow;
	}
}