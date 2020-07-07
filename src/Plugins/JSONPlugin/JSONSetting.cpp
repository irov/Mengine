#include "JSONSetting.h"

#include "Interface/FileServiceInterface.h"

#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    JSONSetting::JSONSetting()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JSONSetting::~JSONSetting()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONSetting::initialize( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DocumentPtr & _doc )
    {
        m_fileGroup = _fileGroup;
        m_filePath = _filePath;

        JSONStorageInterfacePtr storage = JSON_SERVICE()
            ->loadJSON( _fileGroup, _filePath, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( storage );

        m_storage = storage;

        FILE_SERVICE()
            ->setFileModifyHook( _fileGroup, _filePath, [this, _fileGroup, _filePath, _doc]()
        {
            JSONStorageInterfacePtr storage = JSON_SERVICE()
                ->loadJSON( _fileGroup, _filePath, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( storage );

            m_storage = storage;
        } );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void JSONSetting::finalize()
    {
        m_fileGroup = nullptr;
        m_storage = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    const FileGroupInterfacePtr & JSONSetting::getFileGroup() const
    {
        return m_fileGroup;
    }
    //////////////////////////////////////////////////////////////////////////
    const FilePath & JSONSetting::getFilePath() const
    {
        return m_filePath;
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
#ifdef MENGINE_DEBUG
        //////////////////////////////////////////////////////////////////////////
        static bool exist_value( const jpp::object & _j, const Char * _key )
        {
            jpp::object k;
            if( _j.exist( _key, &k ) == false )
            {
                return false;
            }

            bool result = k.exist( "value", nullptr );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool exist_value_attribute( const jpp::object & _j, const Char * _key, const Char * _attribute )
        {
            jpp::object k;
            if( _j.exist( _key, &k ) == false )
            {
                return false;
            }

            jpp::object v;
            if( k.exist( "value", &v ) == false )
            {
                return false;
            }

            jpp::e_type vt = v.type();

            if( vt != jpp::e_type::JPP_OBJECT )
            {
                return false;
            }

            if( v.exist( _attribute, nullptr ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool test_value( const jpp::object & _j, const Char * _key, jpp::e_type _type )
        {
            jpp::object k;
            if( _j.exist( _key, &k ) == false )
            {
                return true;
            }

            jpp::object v;
            if( k.exist( "value", &v ) == false )
            {
                return false;
            }

            jpp::e_type t = v.type();

            if( t != _type )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool test_value_attribute( const jpp::object & _j, const Char * _key, const Char * _attribute, jpp::e_type _type )
        {
            jpp::object k;
            if( _j.exist( _key, &k ) == false )
            {
                return true;
            }

            jpp::object v;
            if( k.exist( "value", &v ) == false )
            {
                return false;
            }

            jpp::e_type vt = v.type();

            if( vt != jpp::e_type::JPP_OBJECT )
            {
                return false;
            }

            jpp::object a;
            if( v.exist( _attribute, &a ) == false )
            {
                return false;
            }

            jpp::e_type t = a.type();

            if( t != _type )
            {
                return false;
            }

            return true;
        }
#endif
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static T get_value( const jpp::object & _j, const Char * _key, T _default )
        {
            jpp::object k;
            if( _j.exist( _key, &k ) == false )
            {
                return _default;
            }

            T value = k.get( "value", _default );

            return value;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONSetting::getValue( const Char * _key, bool _default ) const
    {
        const jpp::object & j = m_storage->getJSON();

        MENGINE_ASSERTION_FATAL( Detail::test_value( j, _key, jpp::e_type::JPP_FALSE ) == true
            || Detail::test_value( j, _key, jpp::e_type::JPP_TRUE ) == true
            || Detail::test_value( j, _key, jpp::e_type::JPP_INTEGER ) == true );

        bool value = Detail::get_value( j, _key, _default );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t JSONSetting::getValue( const Char * _key, int32_t _default ) const
    {
        const jpp::object & j = m_storage->getJSON();

        MENGINE_ASSERTION_FATAL( Detail::test_value( j, _key, jpp::e_type::JPP_INTEGER ) == true );
        
        int32_t value = Detail::get_value( j, _key, _default );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t JSONSetting::getValue( const Char * _key, uint32_t _default ) const
    {
        const jpp::object & j = m_storage->getJSON();

        MENGINE_ASSERTION_FATAL( Detail::test_value( j, _key, jpp::e_type::JPP_INTEGER ) == true, "key ['%s'] is not integer"
            , _key 
        );

        uint32_t value = Detail::get_value( j, _key, _default );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    float JSONSetting::getValue( const Char * _key, float _default ) const
    {
        const jpp::object & j = m_storage->getJSON();

        MENGINE_ASSERTION_FATAL( Detail::test_value( j, _key, jpp::e_type::JPP_REAL ) == true, "key ['%s'] is not real"
            , _key
        );

        float value = Detail::get_value( j, _key, _default );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * JSONSetting::getValue( const Char * _key, const Char * _default ) const
    {
        const jpp::object & j = m_storage->getJSON();

        MENGINE_ASSERTION_FATAL( Detail::test_value( j, _key, jpp::e_type::JPP_STRING ) == true, "key ['%s'] is not string"
            , _key
        );

        const Char * value = Detail::get_value( j, _key, _default );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec2f JSONSetting::getValue( const Char * _key, const mt::vec2f & _default ) const
    {
        const jpp::object & j = m_storage->getJSON();

        MENGINE_ASSERTION_FATAL( Detail::test_value( j, _key, jpp::e_type::JPP_OBJECT ) == true, "key ['%s'] is not object"
            , _key
        );

        jpp::object k;
        if( j.exist( _key, &k ) == false )
        {
            return _default;
        }

        MENGINE_ASSERTION_FATAL( Detail::exist_value( j, _key ) == true );
        MENGINE_ASSERTION_FATAL( Detail::exist_value_attribute( j, _key, "x" ) == true );
        MENGINE_ASSERTION_FATAL( Detail::exist_value_attribute( j, _key, "y" ) == true );
        MENGINE_ASSERTION_FATAL( Detail::test_value_attribute( j, _key, "x", jpp::e_type::JPP_REAL ) == true );
        MENGINE_ASSERTION_FATAL( Detail::test_value_attribute( j, _key, "y", jpp::e_type::JPP_REAL ) == true );

        jpp::object jv2 = k["value"];

        mt::vec2f value( jv2["x"], jv2["y"] );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec3f JSONSetting::getValue( const Char * _key, const mt::vec3f & _default ) const
    {
        const jpp::object & j = m_storage->getJSON();

        MENGINE_ASSERTION_FATAL( Detail::test_value( j, _key, jpp::e_type::JPP_OBJECT ) == true, "key ['%s'] is not object"
            , _key
        );

        jpp::object k;
        if( j.exist( _key, &k ) == false )
        {
            return _default;
        }

        MENGINE_ASSERTION_FATAL( Detail::exist_value( j, _key ) == true );
        MENGINE_ASSERTION_FATAL( Detail::exist_value_attribute( j, _key, "x" ) == true );
        MENGINE_ASSERTION_FATAL( Detail::exist_value_attribute( j, _key, "y" ) == true );
        MENGINE_ASSERTION_FATAL( Detail::exist_value_attribute( j, _key, "z" ) == true );
        MENGINE_ASSERTION_FATAL( Detail::test_value_attribute( j, _key, "x", jpp::e_type::JPP_REAL ) == true );
        MENGINE_ASSERTION_FATAL( Detail::test_value_attribute( j, _key, "y", jpp::e_type::JPP_REAL ) == true );
        MENGINE_ASSERTION_FATAL( Detail::test_value_attribute( j, _key, "z", jpp::e_type::JPP_REAL ) == true );

        jpp::object jv3 = k["value"];

        mt::vec3f value( jv3["x"], jv3["y"], jv3["z"] );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    Color JSONSetting::getValue( const Char * _key, const Color & _default ) const
    {
        const jpp::object & j = m_storage->getJSON();

        MENGINE_ASSERTION_FATAL( Detail::test_value( j, _key, jpp::e_type::JPP_OBJECT ) == true, "key ['%s'] is not real"
            , _key 
        );

        jpp::object k;
        if( j.exist( _key, &k ) == false )
        {
            return _default;
        }

        MENGINE_ASSERTION_FATAL( Detail::exist_value( j, _key ) == true );
        MENGINE_ASSERTION_FATAL( Detail::exist_value_attribute( j, _key, "r" ) == true );
        MENGINE_ASSERTION_FATAL( Detail::exist_value_attribute( j, _key, "g" ) == true );
        MENGINE_ASSERTION_FATAL( Detail::exist_value_attribute( j, _key, "b" ) == true );
        MENGINE_ASSERTION_FATAL( Detail::test_value_attribute( j, _key, "r", jpp::e_type::JPP_REAL ) == true );
        MENGINE_ASSERTION_FATAL( Detail::test_value_attribute( j, _key, "g", jpp::e_type::JPP_REAL ) == true );
        MENGINE_ASSERTION_FATAL( Detail::test_value_attribute( j, _key, "b", jpp::e_type::JPP_REAL ) == true );
        
        jpp::object jc = k["value"];

        Color value( jc["r"], jc["g"], jc["b"], jc.get( "a", 1.f ) );

        return value;
    }
}