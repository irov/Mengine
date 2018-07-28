#include "PluginBase.h"

#include "Interface/ModuleInterface.h"
#include "Interface/ScriptSystemInterface.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	PluginBase::PluginBase()
        : m_dynamicLoad( false )
		, m_initialize( false )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	PluginBase::~PluginBase()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void PluginBase::setDynamicLoad( bool _dynamicLoad )
	{
		m_dynamicLoad = _dynamicLoad;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PluginBase::isDynamicLoad() const
	{
		return m_dynamicLoad;
	}
    //////////////////////////////////////////////////////////////////////////
    bool PluginBase::avaliable()
    {
        bool successful = this->_avaliable();

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PluginBase::_avaliable()
    {
        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool PluginBase::initialize()
	{
		if( m_initialize == true )
		{
			return true;
		}

        if( SERVICE_EXIST( Mengine::ScriptServiceInterface ) == true )
        {
            pybind::kernel_interface * kernel = SCRIPT_SERVICE()
                ->getKernel();

            pybind::set_kernel( kernel );
        }

		bool successful = this->_initialize();

		m_initialize = successful;

		return m_initialize;
	}
	//////////////////////////////////////////////////////////////////////////
	void PluginBase::finalize()
	{
		if( m_initialize == false )
		{
			return;
		}

		m_initialize = false;

		this->_finalize();

		for (VectorModuleFactory::iterator
			it = m_moduleFactories.begin(),
			it_end = m_moduleFactories.end();
			it != it_end;
			++it)
		{
			const ConstString & moduleFactory = *it;

			MODULE_SERVICE()
				->unregisterModule(moduleFactory);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool PluginBase::isInitialize() const
	{
		return m_initialize;
	}
	//////////////////////////////////////////////////////////////////////////
	void PluginBase::destroy()
	{
		this->_destroy();

        bool dynamicLoad = m_dynamicLoad;

        delete this;

		if( dynamicLoad == true )
		{
			stdex_allocator_finalize();
		}        
	}
	//////////////////////////////////////////////////////////////////////////
	bool PluginBase::_initialize()
	{
		///Empty

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PluginBase::_finalize()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	bool PluginBase::addModuleFactory(const ConstString & _name, const ModuleFactoryInterfacePtr & _factory)
	{	
		if (MODULE_SERVICE()
			->registerModule(_name, _factory) == false)
		{
			return false;
		}

		m_moduleFactories.emplace_back(_name);

		return true;
	}
}