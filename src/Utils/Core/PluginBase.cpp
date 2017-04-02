#	include "PluginBase.h"

namespace Menge
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
        return this->_avaliable();
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

		for (TVectorModuleFactory::iterator
			it = m_moduleFactories.begin(),
			it_end = m_moduleFactories.end();
			it != it_end;
			++it)
		{
			const ConstString & moduleFactory = *it;

			MODULE_SERVICE(m_serviceProvider)
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
		_factory->setServiceProvider(m_serviceProvider);

		if (MODULE_SERVICE(m_serviceProvider)
			->registerModule(_name, _factory) == false)
		{
			return false;
		}

		m_moduleFactories.push_back(_name);

		return true;
	}
}