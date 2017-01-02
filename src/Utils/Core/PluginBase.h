#	pragma once

#	include "Interface/PluginInterface.h"

namespace Menge
{
	class PluginBase
		: public ServantBase<PluginInterface>
	{
	public:
		PluginBase();
		~PluginBase();

	public:
		void setDynamicLoad( bool _dynamicLoad ) override;
		bool isDynamicLoad() const override;
		
	private:
		bool initialize() override;
		void finalize() override;

	protected:
		bool isInitialize() const override;

	protected:
		void destroy() override;

	protected:
		virtual bool _initialize();
		virtual void _finalize();
		virtual void _destroy();

	protected:
		bool m_dynamicLoad;
		bool m_initialize;
	};
}