#	pragma once

#	include "Interface/PluginInterface.h"

namespace Menge
{
	class Cal3dCoreModel;

	class PluginCal3d
		: public PluginInterface
	{
	public:
		PluginCal3d();

	protected:
		bool initialize( ServiceProviderInterface * _provider ) override;
		void destroy() override;

	public:
		Cal3dCoreModel * createCal3dCoreModel();
		void destroyCal3dCoreModel( Cal3dCoreModel * _coreModel );

	protected:
        ServiceProviderInterface * m_serviceProvider;
	};
}