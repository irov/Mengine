#	include "GOAPPlugin.h"

#	include "Interface/StringizeInterface.h"

#	include "Logger/Logger.h"

#   include "PybindChain.h"
#   include "PybindSource.h"
#   include "PybindTask.h"
#   include "PybindTaskGenerator.h"

#   include "pybind/pybind.hpp"

namespace pybind
{
    template<typename T>
    struct extract_specialized<GOAP::IntrusivePtr<T> >
    {
        T * operator () ( kernel_interface * _kernel, PyObject * _obj )
        {
            T * value = nullptr;

            if( extract_value( _kernel, _obj, value, true ) == false )
            {
                const std::type_info & tinfo = typeid(T *);

                const char * type_name = tinfo.name();

                pybind::log( "extract_value<intrusive>: extract invalid %s:%s not cast to '%s'"
                    , pybind::object_repr( _obj )
                    , pybind::object_repr_type( _obj )
                    , type_name
                );
            }

            return value;
        }
    };

    template<typename T>
    struct ptr_throw_specialized < GOAP::IntrusivePtr<T> >
    {
        PyObject * operator () ( kernel_interface * _kernel, const GOAP::IntrusivePtr<T> & _t )
        {
            T * t_ptr = _t.get();

            return ptr_throw_i( _kernel, t_ptr );
        }
    };
}
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( GOAP, Menge::GOAPPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	GOAPPlugin::GOAPPlugin()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool GOAPPlugin::_initialize()
	{
        pybind::kernel_interface * kernel = pybind::get_kernel();

        pybind::interface_<PybindChain>( kernel, "PybindChain", false )
            .def_smart_pointer()
            .def_bindable()
            .def( "run", &PybindChain::run )
            .def( "skip", &PybindChain::skip )
            .def( "cancel", &PybindChain::cancel )
            .def( "isComplete", &PybindChain::isComplete )
            ;

        pybind::def_functor( kernel, "makeChain", this, &GOAPPlugin::makeChain );

        pybind::def_functor( kernel, "registryTask", this, &GOAPPlugin::registryTask );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void GOAPPlugin::_finalize()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    PybindChainPtr GOAPPlugin::makeChain( const PybindSourcePtr & _source )
    { 
        PybindChainPtr chain = new PybindChain( _source );

        return chain;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GOAPPlugin::registryTask( const ConstString & _name, const pybind::object & _obj )
    {
        TMapPybindTaskGenerator::iterator it_found = m_generators.find( _name );

        if( it_found != m_generators.end() )
        {
            return false;
        }

        PybindTaskGeneratorPtr generator = new PybindTaskGenerator();

        if( generator->initialize( _obj ) == false )
        {
            return false;
        }

        m_generators.insert( std::make_pair( _name, generator ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    PybindTaskPtr GOAPPlugin::generateTask( const ConstString & _name )
    {
        TMapPybindTaskGenerator::iterator it_found = m_generators.find( _name );

        if( it_found == m_generators.end() )
        {
            return nullptr;
        }

        const PybindTaskGeneratorPtr & generator = it_found->second;

        PybindTaskPtr task = generator->generate();

        return task;
    }
}
