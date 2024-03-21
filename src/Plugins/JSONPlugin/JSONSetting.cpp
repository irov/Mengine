#include "JSONSetting.h"

#if defined(MENGINE_DEBUG)
#   include "Plugins/FileModifyHookPlugin/FileModifyHookInterface.h"
#endif

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionJSONInvalid.h"
#include "Kernel/JSONHelper.h"

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
    bool JSONSetting::initialize( const ContentInterfacePtr & _content, const DocumentInterfacePtr & _doc )
    {
        m_content = _content;

        const FileGroupInterfacePtr & fileGroup = m_content->getFileGroup();
        const FilePath & filePath = m_content->getFilePath();

        jpp::object json = Helper::loadJSONFile( fileGroup, filePath, _doc );

        MENGINE_ASSERTION_JSON_INVALID( json );

        m_json = json;

#if defined(MENGINE_DEBUG)
        FILEMODIFYHOOK_SERVICE()
            ->setFileModifyHook( fileGroup, filePath, [this, fileGroup, filePath, _doc]()
        {
            jpp::object new_json = Helper::loadJSONFile( fileGroup, filePath, _doc );

            MENGINE_ASSERTION_JSON_INVALID( new_json );

            m_json = new_json;
        } );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void JSONSetting::finalize()
    {
#if defined(MENGINE_DEBUG)
        const FileGroupInterfacePtr & fileGroup = m_content->getFileGroup();
        const FilePath & filePath = m_content->getFilePath();

        FILEMODIFYHOOK_SERVICE()
            ->removeFileModifyHook( fileGroup, filePath );
#endif

        m_content = nullptr;
        m_json = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    const ContentInterfacePtr & JSONSetting::getContent() const
    {
        return m_content;
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
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
        MENGINE_ASSERTION_FATAL( Detail::test_value( m_json, _key, jpp::e_type::JPP_FALSE ) == true
            || Detail::test_value( m_json, _key, jpp::e_type::JPP_TRUE ) == true
            || Detail::test_value( m_json, _key, jpp::e_type::JPP_INTEGER ) == true
            , "key ['%s'] is not boolean"
            , _key );

        bool value = Detail::get_value( m_json, _key, _default );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t JSONSetting::getValue( const Char * _key, int32_t _default ) const
    {
        MENGINE_ASSERTION_FATAL( Detail::test_value( m_json, _key, jpp::e_type::JPP_INTEGER ) == true, "key ['%s'] is not integer"
            , _key
        );

        int32_t value = Detail::get_value( m_json, _key, _default );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t JSONSetting::getValue( const Char * _key, uint32_t _default ) const
    {
        MENGINE_ASSERTION_FATAL( Detail::test_value( m_json, _key, jpp::e_type::JPP_INTEGER ) == true, "key ['%s'] is not integer"
            , _key
        );

        uint32_t value = Detail::get_value( m_json, _key, _default );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    float JSONSetting::getValue( const Char * _key, float _default ) const
    {
        MENGINE_ASSERTION_FATAL( Detail::test_value( m_json, _key, jpp::e_type::JPP_REAL ) == true, "key ['%s'] is not real"
            , _key
        );

        float value = Detail::get_value( m_json, _key, _default );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * JSONSetting::getValue( const Char * _key, const Char * _default ) const
    {
        MENGINE_ASSERTION_FATAL( Detail::test_value( m_json, _key, jpp::e_type::JPP_STRING ) == true, "key ['%s'] is not string"
            , _key
        );

        const Char * value = Detail::get_value( m_json, _key, _default );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec2f JSONSetting::getValue( const Char * _key, const mt::vec2f & _default ) const
    {
        MENGINE_ASSERTION_FATAL( Detail::test_value( m_json, _key, jpp::e_type::JPP_OBJECT ) == true, "key ['%s'] is not object"
            , _key
        );

        jpp::object k;
        if( m_json.exist( _key, &k ) == false )
        {
            return _default;
        }

        MENGINE_ASSERTION_FATAL( Detail::exist_value( m_json, _key ) == true, "key ['%s'] is not exist"
            , _key
        );

        MENGINE_ASSERTION_FATAL( Detail::exist_value_attribute( m_json, _key, "x" ) == true, "key ['%s'] attribute 'x' is not exist"
            , _key
        );

        MENGINE_ASSERTION_FATAL( Detail::exist_value_attribute( m_json, _key, "y" ) == true, "key ['%s'] attribute 'y' is not exist"
            , _key
        );

        MENGINE_ASSERTION_FATAL( Detail::test_value_attribute( m_json, _key, "x", jpp::e_type::JPP_REAL ) == true, "key ['%s'] attribute 'x' is not real"
            , _key
        );

        MENGINE_ASSERTION_FATAL( Detail::test_value_attribute( m_json, _key, "y", jpp::e_type::JPP_REAL ) == true, "key ['%s'] attribute 'y' is not real"
            , _key
        );

        jpp::object jv2 = k["value"];

        mt::vec2f value( jv2["x"], jv2["y"] );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec3f JSONSetting::getValue( const Char * _key, const mt::vec3f & _default ) const
    {
        MENGINE_ASSERTION_FATAL( Detail::test_value( m_json, _key, jpp::e_type::JPP_OBJECT ) == true, "key ['%s'] is not object"
            , _key
        );

        jpp::object k;
        if( m_json.exist( _key, &k ) == false )
        {
            return _default;
        }

        MENGINE_ASSERTION_FATAL( Detail::exist_value( m_json, _key ) == true, "key ['%s'] is not exist"
            , _key
        );

        MENGINE_ASSERTION_FATAL( Detail::exist_value_attribute( m_json, _key, "x" ) == true, "key ['%s'] attribute 'x' is not exist"
            , _key
        );

        MENGINE_ASSERTION_FATAL( Detail::exist_value_attribute( m_json, _key, "y" ) == true, "key ['%s'] attribute 'y' is not exist"
            , _key
        );

        MENGINE_ASSERTION_FATAL( Detail::exist_value_attribute( m_json, _key, "z" ) == true, "key ['%s'] attribute 'z' is not exist"
            , _key
        );

        MENGINE_ASSERTION_FATAL( Detail::test_value_attribute( m_json, _key, "x", jpp::e_type::JPP_REAL ) == true, "key ['%s'] attribute 'x' is not real"
            , _key
        );

        MENGINE_ASSERTION_FATAL( Detail::test_value_attribute( m_json, _key, "y", jpp::e_type::JPP_REAL ) == true, "key ['%s'] attribute 'y' is not real"
            , _key
        );

        MENGINE_ASSERTION_FATAL( Detail::test_value_attribute( m_json, _key, "z", jpp::e_type::JPP_REAL ) == true, "key ['%s'] attribute 'z' is not real"
            , _key
        );

        jpp::object jv3 = k["value"];

        mt::vec3f value( jv3["x"], jv3["y"], jv3["z"] );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    Color JSONSetting::getValue( const Char * _key, const Color & _default ) const
    {
        MENGINE_ASSERTION_FATAL( Detail::test_value( m_json, _key, jpp::e_type::JPP_OBJECT ) == true, "key ['%s'] is not real"
            , _key
        );

        jpp::object k;
        if( m_json.exist( _key, &k ) == false )
        {
            return _default;
        }

        MENGINE_ASSERTION_FATAL( Detail::exist_value( m_json, _key ) == true, "key ['%s'] is not exist"
            , _key
        );

        MENGINE_ASSERTION_FATAL( Detail::exist_value_attribute( m_json, _key, "r" ) == true, "key ['%s'] attribute 'r' is not exist"
            , _key
        );

        MENGINE_ASSERTION_FATAL( Detail::exist_value_attribute( m_json, _key, "g" ) == true, "key ['%s'] attribute 'g' is not exist"
            , _key
        );

        MENGINE_ASSERTION_FATAL( Detail::exist_value_attribute( m_json, _key, "b" ) == true, "key ['%s'] attribute 'b' is not exist"
            , _key
        );

        MENGINE_ASSERTION_FATAL( Detail::test_value_attribute( m_json, _key, "r", jpp::e_type::JPP_REAL ) == true, "key ['%s'] attribute 'r' is not real"
            , _key
        );

        MENGINE_ASSERTION_FATAL( Detail::test_value_attribute( m_json, _key, "g", jpp::e_type::JPP_REAL ) == true, "key ['%s'] attribute 'g' is not real"
            , _key
        );

        MENGINE_ASSERTION_FATAL( Detail::test_value_attribute( m_json, _key, "b", jpp::e_type::JPP_REAL ) == true, "key ['%s'] attribute 'b' is not real"
            , _key
        );

        jpp::object jc = k["value"];

        Color value( jc["r"], jc["g"], jc["b"], jc.get( "a", 1.f ) );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
}
