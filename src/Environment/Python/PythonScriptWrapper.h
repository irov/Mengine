#pragma once

#include "Interface/ScriptWrapperInterface.h"
#include "Interface/ScriptServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonDocumentTraceback.h"

#include "Kernel/Node.h"
#include "Kernel/Factorable.h"
#include "Kernel/Scriptable.h"
#include "Kernel/AssertionType.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/VocabularyHelper.h"

#include "Config/Typeinfo.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        template<class T>
        struct ScriptExtract
            : public pybind::interface_<T>::extract_type_ptr
        {
            PyObject * wrap( pybind::kernel_interface * _kernel, typename pybind::interface_<T>::extract_type_ptr::TCastRef _self ) override
            {
                if( _self == nullptr )
                {
                    return _kernel->ret_none();
                }

                Scriptable * bindable = static_cast<Scriptable *>(_self);

                PyObject * py_obj = bindable->getEmbed( _kernel );

                return py_obj;
            }
        };
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    class PythonScriptWrapper
        : public ScriptWrapperInterface
        , public Factorable
    {
    public:
        PythonScriptWrapper( pybind::kernel_interface * _kernel )
            : m_kernel( _kernel )
        {
            pybind::allocator_interface * allocator = _kernel->get_allocator();

            pybind::type_cast_ptr cast = allocator->newT<Detail::ScriptExtract<T>>();

            pybind::registration_type_cast<T>(m_kernel, cast);

            const pybind::class_type_scope_interface_ptr & scope = m_kernel->get_class_type_scope_t<T>();

            if( scope == nullptr )
            {
                return;
            }

            m_scope = scope;
        }

        ~PythonScriptWrapper() override
        {
            pybind::unregistration_type_cast<T>(m_kernel);
        }

    protected:
        PyObject * wrap( Scriptable * _scriptable, PyObject * _embed ) override
        {
            if( _embed != nullptr )
            {
                m_kernel->incref( _embed );

                return _embed;
            }

            MENGINE_ASSERTION_TYPE( _scriptable, T *, "invalid wrap type '%s' (doc %s)"
                , MENGINE_TYPEINFO_NAME( T )
                , MENGINE_DOCUMENT_STR( MENGINE_DOCUMENT_PYBIND )
            );
            
            T * obj = static_cast<T *>(_scriptable);

            PyObject * py_obj = m_kernel->scope_create_holder_t( obj );

            //pybind::set_attr( py_embedded, "Mengine_name", pybind::ptr(_node->getName()) );
            //pybind::set_attr( py_embedded, "Mengine_type", pybind::ptr(_node->getType()) );
            //pybind::set_attr( py_embedded, "Mengine_tag", pybind::ptr(_node->getTag()) );

#if defined(MENGINE_DEBUG)
            if( m_kernel->is_object_bindable( py_obj ) == false )
            {
                MENGINE_THROW_EXCEPTION( "'%s' but scope not setup bindable"
                    , m_kernel->object_repr_type( py_obj ).c_str()
                );

                return nullptr;
            }
#endif

            return py_obj;
        }

        void unwrap( PyObject * _obj ) override
        {
            void * impl = m_kernel->get_class_impl( _obj );

            if( impl != nullptr && m_kernel->is_class_weak( _obj ) == false )
            {
                m_scope->clear_bindable( impl );
                m_scope->decref_smart_pointer( impl );
            }

            m_kernel->unwrap( _obj );
        }

        bool isWrap( PyObject * _obj ) const override
        {
            bool result = m_kernel->is_wrap( _obj );

            return result;
        }

    protected:
        pybind::kernel_interface * m_kernel;

        pybind::class_type_scope_interface_ptr m_scope;
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        void registerScriptWrapping( pybind::kernel_interface * _kernel, const DocumentInterfacePtr & _doc )
        {
            const ConstString & type = T::getFactorableType();

            VOCABULARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ScriptWrapping" ), type, Helper::makeFactorableUnique<PythonScriptWrapper<T>>( _doc, _kernel ), _doc );
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        void registerScriptWrappingEx( pybind::kernel_interface * _kernel, const ConstString & _type, const DocumentInterfacePtr & _doc )
        {
            VOCABULARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ScriptWrapping" ), _type, Helper::makeFactorableUnique<PythonScriptWrapper<T>>( _doc, _kernel ), _doc );
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE void unregisterScriptWrappingEx( const ConstString & _type )
        {
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ScriptWrapping" ), _type );
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        void unregisterScriptWrapping()
        {
            const ConstString & type = T::getFactorableType();

            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ScriptWrapping" ), type );
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
}
