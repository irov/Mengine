#include "MemoryConfig.h"

#include "Kernel/Exception.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/Stringalized.h"

#include "Config/ArrayString.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        bool s_getRecord( const VectorRecords & _records, const Char * _section, const Char * _key, const RecordDesc ** _record )
        {
            for( const RecordDesc & record : _records )
            {
                if( record.section != _section )
                {
                    continue;
                }

                if( record.key != _key )
                {
                    continue;
                }

                *_record = &record;

                return true;
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        typedef Vector<const RecordDesc *> VectorRecordRefs;
        //////////////////////////////////////////////////////////////////////////
        void s_filterRecords( const VectorRecords & _records, const Char * _section, const Char * _key, VectorRecordRefs & _refs )
        {
            for( const RecordDesc & record : _records )
            {
                if( record.section != _section )
                {
                    continue;
                }

                if( record.key != _key )
                {
                    continue;
                }

                _refs.emplace_back( &record );
            }
        }        
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static bool s_hasValueT( const VectorRecords & _records, const Tags & _tags, const Char * _section, const Char * _key, T * _value )
        {
            ArrayString<128> platform_section;
            platform_section.append( _section );

            const VectorConstString & tags = _tags.getValues();

            for( const ConstString & tag : tags )
            {
                platform_section.append( '-' );
                platform_section.append( tag );

                const RecordDesc * tag_record;
                if( s_getRecord( _records, platform_section.c_str(), _key, &tag_record ) == false )
                {
                    continue;
                }

                const Char * tag_value_string = tag_record->value.c_str();

                if( Helper::stringalized( tag_value_string, _value ) == false )
                {
                    return false;
                }

                return true;
            }

            const RecordDesc * record;
            if( s_getRecord( _records, _section, _key, &record ) == false )
            {
                return false;
            }

            const Char * value_string = record->value.c_str();

            if( Helper::stringalized( value_string, _value ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static T s_getValueT( const VectorRecords & _records, const Tags & _tags, const Char * _section, const Char * _key, T _default )
        {
            T value;

            if( s_hasValueT( _records, _tags, _section, _key, &value ) == false )
            {
                return _default;
            }

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static void s_calcValuesT( const VectorRecords & _records, const Tags & _tags, const Char * _section, const Char * _key, Vector<T> & _values )
        {
            ArrayString<128> platform_section;
            platform_section.append( _section );

            const VectorConstString & tags = _tags.getValues();

            VectorRecordRefs record_refs;

            for( const ConstString & tag : tags )
            {
                platform_section.append( '-' );
                platform_section.append( tag );

                s_filterRecords( _records, platform_section.c_str(), _key, record_refs );
            }

            if( record_refs.empty() == true )
            {
                s_filterRecords( _records, _section, _key, record_refs );
            }

            for( const RecordDesc * desc : record_refs )
            {
                const Char * value_string = desc->value.c_str();

                T value;
                if( Helper::stringalized( value_string, &value ) == false )
                {
                    continue;
                }

                _values.emplace_back( value );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryConfig::MemoryConfig()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryConfig::~MemoryConfig()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void MemoryConfig::setPlatformTags( const Tags & _platformTags )
    {
        m_platformTags = _platformTags;
    }
    //////////////////////////////////////////////////////////////////////////
    const Tags & MemoryConfig::getPlatformTags() const
    {
        return m_platformTags;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryConfig::existValue( const Char * _section, const Char * _key ) const
    {
        ArrayString<128> platform_section;
        platform_section.append( _section );

        const VectorConstString & tags = m_platformTags.getValues();

        for( const ConstString & tag : tags )
        {
            platform_section.append( '-' );
            platform_section.append( tag );

            const RecordDesc * tag_record;
            if( Detail::s_getRecord( m_records, platform_section.c_str(), _key, &tag_record ) == false )
            {
                continue;
            }

            return true;
        }

        const RecordDesc * record;
        if( Detail::s_getRecord( m_records, _section, _key, &record ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryConfig::hasValue( const Char * _section, const Char * _key, bool * _value ) const
    {
        return Detail::s_hasValueT( m_records, m_platformTags, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryConfig::hasValue( const Char * _section, const Char * _key, int32_t * _value ) const
    {
        return Detail::s_hasValueT( m_records, m_platformTags, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryConfig::hasValue( const Char * _section, const Char * _key, uint32_t * _value ) const
    {
        return Detail::s_hasValueT( m_records, m_platformTags, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryConfig::hasValue( const Char * _section, const Char * _key, uint64_t * _value ) const
    {
        return Detail::s_hasValueT( m_records, m_platformTags, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryConfig::hasValue( const Char * _section, const Char * _key, float * _value ) const
    {
        return Detail::s_hasValueT( m_records, m_platformTags, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryConfig::hasValue( const Char * _section, const Char * _key, double * _value ) const
    {
        return Detail::s_hasValueT( m_records, m_platformTags, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryConfig::hasValue( const Char * _section, const Char * _key, const Char ** _value ) const
    {
        return Detail::s_hasValueT( m_records, m_platformTags, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryConfig::hasValue( const Char * _section, const Char * _key, ConstString * _value ) const
    {
        return Detail::s_hasValueT( m_records, m_platformTags, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryConfig::hasValue( const Char * _section, const Char * _key, FilePath * _value ) const
    {
        return Detail::s_hasValueT( m_records, m_platformTags, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryConfig::hasValue( const Char * _section, const Char * _key, Tags * _value ) const
    {
        return Detail::s_hasValueT( m_records, m_platformTags, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryConfig::hasValue( const Char * _section, const Char * _key, Resolution * _value ) const
    {
        return Detail::s_hasValueT( m_records, m_platformTags, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryConfig::hasValue( const Char * _section, const Char * _key, Color * _value ) const
    {
        return Detail::s_hasValueT( m_records, m_platformTags, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryConfig::getValue( const Char * _section, const Char * _key, bool _default ) const
    {
        return Detail::s_getValueT( m_records, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t MemoryConfig::getValue( const Char * _section, const Char * _key, int32_t _default ) const
    {
        return Detail::s_getValueT( m_records, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MemoryConfig::getValue( const Char * _section, const Char * _key, uint32_t _default ) const
    {
        return Detail::s_getValueT( m_records, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t MemoryConfig::getValue( const Char * _section, const Char * _key, uint64_t _default ) const
    {
        return Detail::s_getValueT( m_records, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    float MemoryConfig::getValue( const Char * _section, const Char * _key, float _default ) const
    {
        return Detail::s_getValueT( m_records, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    double MemoryConfig::getValue( const Char * _section, const Char * _key, double _default ) const
    {
        return Detail::s_getValueT( m_records, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * MemoryConfig::getValue( const Char * _section, const Char * _key, const Char * _default ) const
    {
        return Detail::s_getValueT( m_records, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    ConstString MemoryConfig::getValue( const Char * _section, const Char * _key, const ConstString & _default ) const
    {
        return Detail::s_getValueT( m_records, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    FilePath MemoryConfig::getValue( const Char * _section, const Char * _key, const FilePath & _default ) const
    {
        return Detail::s_getValueT( m_records, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    Tags MemoryConfig::getValue( const Char * _section, const Char * _key, const Tags & _default ) const
    {
        return Detail::s_getValueT( m_records, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    Resolution MemoryConfig::getValue( const Char * _section, const Char * _key, const Resolution & _default ) const
    {
        return Detail::s_getValueT( m_records, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    Color MemoryConfig::getValue( const Char * _section, const Char * _key, const Color & _default ) const
    {
        return Detail::s_getValueT( m_records, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    void MemoryConfig::getValues( const Char * _section, const Char * _key, VectorAspectRatioViewports & _values ) const
    {
        Detail::s_calcValuesT( m_records, m_platformTags, _section, _key, _values );
    }
    //////////////////////////////////////////////////////////////////////////
    void MemoryConfig::getValues( const Char * _section, const Char * _key, VectorFilePath & _values ) const
    {
        Detail::s_calcValuesT( m_records, m_platformTags, _section, _key, _values );
    }
    //////////////////////////////////////////////////////////////////////////
    void MemoryConfig::getValues( const Char * _section, const Char * _key, VectorConstString & _values ) const
    {
        Detail::s_calcValuesT( m_records, m_platformTags, _section, _key, _values );
    }
    //////////////////////////////////////////////////////////////////////////
    void MemoryConfig::getValues( const Char * _section, const Char * _key, VectorString & _values ) const
    {
        Detail::s_calcValuesT( m_records, m_platformTags, _section, _key, _values );
    }
    //////////////////////////////////////////////////////////////////////////
    void MemoryConfig::setValue( const Char * _section, const Char * _key, const Char * _value )
    {
        RecordDesc desc;
        desc.section = _section;
        desc.key = _key;
        desc.value = _value;

        m_records.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryConfig::hasSection( const Char * _section ) const
    {
        for( const RecordDesc & desc : m_records )
        {
            if( desc.section != _section )
            {
                continue;
            }

            return true;
        }

        return false;
    }
}
