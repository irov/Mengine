#include "JSONSetting.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionJSONInvalid.h"
#include "Kernel/JSONHelper.h"
#include "Kernel/FileGroupHelper.h"

namespace Mengine
{
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
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    JSONSetting::JSONSetting()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JSONSetting::~JSONSetting()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONSetting::initialize( const ConstString & _name, const ContentInterfacePtr & _content, const DocumentInterfacePtr & _doc )
    {
        m_name = _name;
        m_content = _content;

        const FileGroupInterfacePtr & fileGroup = m_content->getFileGroup();
        const FilePath & filePath = m_content->getFilePath();

        jpp::object json = Helper::loadJSONFile( fileGroup, filePath, _doc );

        MENGINE_ASSERTION_JSON_INVALID( json, "invalid load '%s'"
            , Helper::getFileGroupFullPath( fileGroup, filePath ).c_str()
        );

        m_json = json;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void JSONSetting::finalize()
    {
        m_content = nullptr;
        m_json = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONSetting::save( const DocumentInterfacePtr & _doc ) const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_content, "content is nullptr" );

        if( Helper::writeJSONContent( m_json, m_content, true, _doc ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & JSONSetting::getName() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    const ContentInterfacePtr & JSONSetting::getContent() const
    {
        return m_content;
    }
    //////////////////////////////////////////////////////////////////////////
    void JSONSetting::foreachKeys( const LambdaKeys & _lambda ) const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_json, "json is nullptr" );

        jpp::object_iterator it = jpp::begin( m_json );
        jpp::object_iterator it_end = jpp::end( m_json );

        for( ; it != it_end; ++it )
        {
            const Char * key = (*it).key;

            _lambda( key );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    ESettingType JSONSetting::getValueType( const Char * _key ) const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_json, "json is nullptr" );
        
        jpp::object k;
        if( m_json.exist( _key, &k ) == false )
        {
            return EST_NONE;
        }

        jpp::object v;
        if( k.exist( "value", &v ) == false )
        {
            return EST_NONE;
        }

        jpp::e_type t = v.type();

        switch( t )
        {
        case jpp::e_type::JPP_FALSE:
        case jpp::e_type::JPP_TRUE:
            {
                return EST_BOOL;
            }break;
        case jpp::e_type::JPP_INTEGER:
            {
                return EST_INTEGER;
            }break;
        case jpp::e_type::JPP_REAL:
            {
                return EST_REAL;
            }break;
        case jpp::e_type::JPP_STRING:
            {
                return EST_STRING;
            }break;
        case jpp::e_type::JPP_OBJECT:
            {
                if( v.exist( "x", nullptr ) == true &&
                    v.exist( "y", nullptr ) == true )
                {
                    if( v.exist( "z", nullptr ) == true )
                    {
                        return EST_VEC3F;
                    }
                    else
                    {
                        return EST_VEC2F;
                    }
                }

                if( v.exist( "r", nullptr ) == true &&
                    v.exist( "g", nullptr ) == true &&
                    v.exist( "b", nullptr ) == true )
                {
                    return EST_COLOR;
                }

                return EST_NONE;
            }break;
        default:
            {
                return EST_NONE;
            }break;
        }

        return EST_NONE;
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONSetting::getValueBoolean( const Char * _key, bool _default ) const
    {
        MENGINE_ASSERTION_FATAL( Detail::test_value( m_json, _key, jpp::e_type::JPP_FALSE ) == true
            || Detail::test_value( m_json, _key, jpp::e_type::JPP_TRUE ) == true
            || Detail::test_value( m_json, _key, jpp::e_type::JPP_INTEGER ) == true
            , "key ['%s'] is not boolean"
            , _key
        );

        bool value = Detail::get_value( m_json, _key, _default );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t JSONSetting::getValueInteger( const Char * _key, int64_t _default ) const
    {
        MENGINE_ASSERTION_FATAL( Detail::test_value( m_json, _key, jpp::e_type::JPP_INTEGER ) == true, "key ['%s'] is not integer"
            , _key
        );

        int64_t value = Detail::get_value( m_json, _key, _default );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    double JSONSetting::getValueFloat( const Char * _key, double _default ) const
    {
        MENGINE_ASSERTION_FATAL( Detail::test_value( m_json, _key, jpp::e_type::JPP_REAL ) == true, "key ['%s'] is not real"
            , _key
        );

        double value = Detail::get_value( m_json, _key, _default );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * JSONSetting::getValueString( const Char * _key, const Char * _default ) const
    {
        MENGINE_ASSERTION_FATAL( Detail::test_value( m_json, _key, jpp::e_type::JPP_STRING ) == true, "key ['%s'] is not string"
            , _key
        );

        const Char * value = Detail::get_value( m_json, _key, _default );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec2f JSONSetting::getValueVec2f( const Char * _key, const mt::vec2f & _default ) const
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
    mt::vec3f JSONSetting::getValueVec3f( const Char * _key, const mt::vec3f & _default ) const
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

        float x = jv3["x"];
        float y = jv3["y"];
        float z = jv3["z"];

        mt::vec3f value( x, y, z );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    Color JSONSetting::getValueColor( const Char * _key, const Color & _default ) const
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

        float r = jc["r"];
        float g = jc["g"];
        float b = jc["b"];
        float a = jc.get( "a", 1.f );

        Color value( r, g, b, a );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONSetting::setValueBoolean( const Char * _key, bool _value )
    {
        jpp::object v;
        if( m_json.exist( _key, &v ) == false )
        {
            return false;
        }

        v.set( "value", _value );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONSetting::setValueInteger( const Char * _key, int64_t _value )
    {
        jpp::object v;
        if( m_json.exist( _key, &v ) == false )
        {
            return false;
        }

        v.set( "value", _value );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONSetting::setValueFloat( const Char * _key, double _value )
    {
        jpp::object v;
        if( m_json.exist( _key, &v ) == false )
        {
            return false;
        }

        v.set( "value", _value );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONSetting::setValueString( const Char * _key, const Char * _value )
    {
        jpp::object v;
        if( m_json.exist( _key, &v ) == false )
        {
            return false;
        }
        
        v.set( "value", _value );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONSetting::setValueVec2f( const Char * _key, const mt::vec2f & _value )
    {
        jpp::object v;
        if( m_json.exist( _key, &v ) == false )
        {
            return false;
        }

        jpp::object xy;
        if( v.exist( "value", &xy ) == false )
        {
            return false;
        }
        
        xy.set( "x", _value.x );
        xy.set( "y", _value.y );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONSetting::setValueVec3f( const Char * _key, const mt::vec3f & _value )
    {
        jpp::object v;
        if( m_json.exist( _key, &v ) == false )
        {
            return false;
        }

        jpp::object xyz;
        if( v.exist( "value", &xyz ) == false )
        {
            return false;
        }

        xyz.set( "x", _value.x );
        xyz.set( "y", _value.y );
        xyz.set( "z", _value.z );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONSetting::setValueColor( const Char * _key, const Color & _value )
    {
        jpp::object v;
        if( m_json.exist( _key, &v ) == false )
        {
            return false;
        }

        jpp::object rgba;
        if( v.exist( "value", &rgba ) == false )
        {
            return false;
        }

        float r = _value.getR();
        float g = _value.getG();
        float b = _value.getB();
        float a = _value.getA();

        rgba.set( "r", r );
        rgba.set( "g", g );
        rgba.set( "b", b );
        rgba.set( "a", a );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
