#	include "PluginCal3d.h"

#	include "Cal3dModel.h"
#	include "Cal3dCoreModel.h"

#	include "ResourceCal3dAnimation.h"
#	include "ResourceCal3dMesh.h"
#	include "ResourceCal3dSkeleton.h"

#	include "Kernel/ScriptClassWrapper.h"
#	include "Kernel/NodePrototypeGenerator.h"
#	include "Kernel/ResourcePrototypeGenerator.h"

#	include "pybind/pybind.hpp"

extern "C" // only required if using g++
{
    //////////////////////////////////////////////////////////////////////////
    bool initPluginMengeCal3d( Menge::PluginInterface ** _plugin )
    {
        *_plugin = new Menge::PluginCal3d();

        return true;
    }
#   ifdef MENGE_PLUGIN_DLL
    ////////////////////////////////////////////////////////////////////////////
    __declspec(dllexport) bool dllCreatePlugin( Menge::PluginInterface ** _plugin )
    {
        return initPluginMengeCal3d( _plugin );
    }
#   endif
}
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	PluginCal3d::PluginCal3d()
		: m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool PluginCal3d::initialize( ServiceProviderInterface * _provider )
	{
		m_serviceProvider = _provider;

		pybind::interface_<Cal3dCoreModel>("Cal3dCoreModel")
			;

		pybind::def_functor( "createCal3dCoreModel", this, &PluginCal3d::createCal3dCoreModel );
		pybind::def_functor( "destroyCal3dCoreModel", this, &PluginCal3d::destroyCal3dCoreModel );

		pybind::interface_<Cal3dModel, pybind::bases<Node> >("Cal3dModel", false)
			.def( "setCoreModel", &Cal3dModel::setCoreModel )
			.def( "attachMesh", &Cal3dModel::attachMesh )
			;

		pybind::interface_<Cal3dCoreModel>("Cal3dCoreModel", false)
			.def( "setResourceSkeleton", &Cal3dCoreModel::setResourceSkeleton )
			.def( "addResourceMesh", &Cal3dCoreModel::addResourceMesh )
			.def( "addResourceAnimation", &Cal3dCoreModel::addResourceAnimation )
			.def( "getMeshId", &Cal3dCoreModel::getMeshId )
			.def( "getAnimationId", &Cal3dCoreModel::getAnimationId )
			.def( "compile", &Cal3dCoreModel::compile )
			.def( "release", &Cal3dCoreModel::release )
			;

		SCRIPT_SERVICE(m_serviceProvider)
			->addWrapping( Helper::stringizeString(m_serviceProvider, "Cal3dModel"), new ScriptClassWrapper<Cal3dModel>() );

		PROTOTYPE_SERVICE(m_serviceProvider)
			->addPrototype( CONST_STRING_LOCAL(m_serviceProvider, "Node"), CONST_STRING_LOCAL(m_serviceProvider, "Cal3dModel"), new NodePrototypeGenerator<Cal3dModel, 128>(m_serviceProvider) );
		
		PROTOTYPE_SERVICE(m_serviceProvider)
			->addPrototype( CONST_STRING_LOCAL(m_serviceProvider, "Resource"), CONST_STRING_LOCAL(m_serviceProvider, "ResourceCal3dAnimation"), new ResourcePrototypeGenerator<ResourceCal3dAnimation, 128>(m_serviceProvider) );

		PROTOTYPE_SERVICE(m_serviceProvider)
			->addPrototype( CONST_STRING_LOCAL(m_serviceProvider, "Resource"), CONST_STRING_LOCAL(m_serviceProvider, "ResourceCal3dMesh"), new ResourcePrototypeGenerator<ResourceCal3dMesh, 128>(m_serviceProvider) );

		PROTOTYPE_SERVICE(m_serviceProvider)
			->addPrototype( CONST_STRING_LOCAL(m_serviceProvider, "Resource"), CONST_STRING_LOCAL(m_serviceProvider, "ResourceCal3dSkeleton"), new ResourcePrototypeGenerator<ResourceCal3dSkeleton, 128>(m_serviceProvider) );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PluginCal3d::destroy()
	{
		delete this;
	}
	//////////////////////////////////////////////////////////////////////////
	Cal3dCoreModel * PluginCal3d::createCal3dCoreModel()
	{
		return new Cal3dCoreModel;
	}
	//////////////////////////////////////////////////////////////////////////
	void PluginCal3d::destroyCal3dCoreModel( Cal3dCoreModel * _coreModel )
	{
		delete _coreModel;
	}

}