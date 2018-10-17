#include "VideoPlugin.h"

#include "Interface/PrototypeManagerInterface.h"
#include "Interface/StringizeInterface.h"

#include "../Plugin/ResourceValidatePlugin/ResourceValidateInterface.h"

#include "SurfaceVideo.h"
#include "VideoResourceValidateVisitor.h"

#include "PythonScriptWrapper/PythonAnimatableEventReceiver.h"

#include "Kernel/ResourcePrototypeGenerator.h"
#include "Kernel/SurfacePrototypeGenerator.h"
#include "Kernel/ScriptWrapper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Video, Mengine::VideoPlugin);
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
	VideoPlugin::VideoPlugin()
    {
    }
	namespace Detail
	{
		//////////////////////////////////////////////////////////////////////////
		PyObject * s_SurfaceVideo_setEventListener(pybind::kernel_interface * _kernel, SurfaceVideo * _surface, PyObject * _args, PyObject * _kwds)
		{
			(void)_args;

			if (_kwds == nullptr)
			{
				return pybind::ret_none();
			}

			pybind::dict py_kwds(_kernel, _kwds);
			Helper::registerAnimatableEventReceiver<>(py_kwds, _surface);

#ifndef NDEBUG
			if (py_kwds.empty() == false)
			{
				for (pybind::dict::iterator
					it = py_kwds.begin(),
					it_end = py_kwds.end();
					it != it_end;
					++it)
				{
					String k = it.key();

					LOGGER_ERROR("SurfaceVideo::setEventListener invalid kwds '%s'\n"
						, k.c_str()
					);
				}

				throw;
			}
#endif

			return pybind::ret_none();
		}
	}
    //////////////////////////////////////////////////////////////////////////
    bool VideoPlugin::_initialize()
    {
		if (PROTOTYPE_SERVICE()
			->addPrototype(STRINGIZE_STRING_LOCAL("Resource"), STRINGIZE_STRING_LOCAL("ResourceVideo"), new FactorableUnique<ResourcePrototypeGenerator<ResourceVideo, 128> >) == false)
		{
			return false;
		}

		if (PROTOTYPE_SERVICE()
			->addPrototype(STRINGIZE_STRING_LOCAL("Surface"), STRINGIZE_STRING_LOCAL("SurfaceVideo"), new FactorableUnique<SurfacePrototypeGenerator<SurfaceVideo, 128> >) == false)
		{
			return false; 
		}

		
		if (SCRIPT_SERVICE()
			->setWrapper(STRINGIZE_STRING_LOCAL("ResourceVideo"), new ScriptWrapper<ResourceVideo>()) == false)
		{
			return false;
		}

		if (SCRIPT_SERVICE()
			->setWrapper(STRINGIZE_STRING_LOCAL("SurfaceVideo"), new ScriptWrapper<SurfaceVideo>()) == false)
		{
			return false;
		}

		pybind::kernel_interface * kernel = SCRIPT_SERVICE()
			->getKernel();

		pybind::set_kernel(kernel);

		pybind::interface_<ResourceVideo, pybind::bases<Resource> >(kernel, "ResourceVideo", false)
			;

		pybind::interface_<SurfaceVideo, pybind::bases<Surface, Eventable, Animatable> >(kernel, "SurfaceVideo", false)
			.def("setResourceVideo", &SurfaceVideo::setResourceVideo)
			.def("getResourceVideo", &SurfaceVideo::getResourceVideo)
			.def("getWidth", &SurfaceVideo::getWidth)
			.def("getHeight", &SurfaceVideo::getHeight)
			.def("getDuration", &SurfaceVideo::getDuration)
			.def_static_native_kernel("setEventListener", &Detail::s_SurfaceVideo_setEventListener)
			;


		VisitorPtr videoValidateVisitor = new FactorableUnique<VideoResourceValidateVisitor>();

		RESOURCEVALIDATE_SERVICE()
			->addResourceValidateVisitor(videoValidateVisitor);

		m_videoValidateVisitor = videoValidateVisitor;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void VideoPlugin::_finalize()
    {
    }
}