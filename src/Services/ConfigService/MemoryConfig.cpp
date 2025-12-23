#include "MemoryConfig.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionNotImplemented.h"
#include "Kernel/Logger.h"
#include "Kernel/Stringalized.h"
#include "Kernel/ArrayString.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static bool getRecord( const MemoryConfig::VectorRecords & _records, const Char * _section, const Char * _key, const MemoryConfig::RecordDesc ** const _record )
        {
            for( const MemoryConfig::RecordDesc & record : _records )
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
        static bool getRecord( MemoryConfig::VectorRecords & _records, const Char * _section, const Char * _key, MemoryConfig::RecordDesc ** const _record )
        {
            for( MemoryConfig::RecordDesc & record : _records )
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
        typedef Vector<const MemoryConfig::RecordDesc *> VectorRecordRefs;
        //////////////////////////////////////////////////////////////////////////
        static void filterRecords( const MemoryConfig::VectorRecords & _records, const Char * _section, const Char * _key, VectorRecordRefs * const _refs )
        {
            for( const MemoryConfig::RecordDesc & record : _records )
            {
                if( record.section != _section )
                {
                    continue;
                }

                if( record.key != _key )
                {
                    continue;
                }

                _refs->emplace_back( &record );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static bool hasValueT( const MemoryConfig::VectorRecords & _records, const Tags & _tags, const Char * _section, const Char * _key, T _default, T * const _value )
        {
            ArrayString<128> platform_section;
            platform_section.append( _section );

            const VectorConstString & tags = _tags.getValues();

            for( const ConstString & tag : tags )
            {
                platform_section.append( '-' );
                platform_section.append( tag );

                const MemoryConfig::RecordDesc * tag_record;
                if( Detail::getRecord( _records, platform_section.c_str(), _key, &tag_record ) == false )
                {
                    continue;
                }

                const Char * tag_value_string = tag_record->value.c_str();

                if( Helper::stringalized( tag_value_string, _value ) == false )
                {
                    *_value = _default;

                    return false;
                }

                return true;
            }

            const MemoryConfig::RecordDesc * record;
            if( Detail::getRecord( _records, _section, _key, &record ) == false )
            {
                *_value = _default;

                return false;
            }

            const Char * value_string = record->value.c_str();

            if( Helper::stringalized( value_string, _value ) == false )
            {
                *_value = _default;

                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static bool setValueT( MemoryConfig::VectorRecords & _records, const Char * _section, const Char * _key, T _value )
        {
            ArrayString<128> platform_section;
            platform_section.append( _section );

            MemoryConfig::RecordDesc * tag_record;
            if( Detail::getRecord( _records, _section, _key, &tag_record ) == false )
            {
                MemoryConfig::RecordDesc desc;
                desc.section.assign( _section );
                desc.key.assign( _key );

                tag_record = &_records.emplace_back( desc );
            }

            Char buffer[1024 + 1] = {'\0'};
            if( Helper::stringalized( _value, buffer, 1024 ) == false )
            {
                return false;
            }

            tag_record->value.assign( buffer );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static void calcValuesT( const MemoryConfig::VectorRecords & _records, const Tags & _tags, const Char * _section, const Char * _key, Vector<T> * const _values )
        {
            ArrayString<128> platform_section;
            platform_section.append( _section );

            const VectorConstString & tags = _tags.getValues();

            VectorRecordRefs record_refs;

            for( const ConstString & tag : tags )
            {
                platform_section.append( '-' );
                platform_section.append( tag );

                Detail::filterRecords( _records, platform_section.c_str(), _key, &record_refs );
            }

            if( record_refs.empty() == true )
            {
                Detail::filterRecords( _records, _section, _key, &record_refs );
            }

            for( const MemoryConfig::RecordDesc * desc : record_refs )
            {
                const Char * value_string = desc->value.c_str();

                T value;
                if( Helper::stringalized( value_string, &value ) == false )
                {
                    continue;
                }

                _values->emplace_back( value );
            }
        }
        //////////////////////////////////////////////////////////////////////////
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
            if( Detail::getRecord( m_records, platform_section.c_str(), _key, &tag_record ) == false )
            {
                continue;
            }

            return true;
        }

        const RecordDesc * record;
        if( Detail::getRecord( m_records, _section, _key, &record ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryConfig::load( const InputStreamInterfacePtr & _stream, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _stream );
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MemoryConfig::unload()
    {
        m_records.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryConfig::hasValue( const Char * _section, const Char * _key, bool _default, bool * const _value ) const
    {
        return Detail::hasValueT( m_records, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryConfig::hasValue( const Char * _section, const Char * _key, int64_t _default, int64_t * const  _value ) const
    {
        return Detail::hasValueT( m_records, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryConfig::hasValue( const Char * _section, const Char * _key, double _default, double * const _value ) const
    {
        return Detail::hasValueT( m_records, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryConfig::hasValue( const Char * _section, const Char * _key, const String & _default, String * const _value ) const
    {
        return Detail::hasValueT( m_records, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryConfig::hasValue( const Char * _section, const Char * _key, const ConstString & _default, ConstString * const _value ) const
    {
        return Detail::hasValueT( m_records, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryConfig::hasValue( const Char * _section, const Char * _key, const FilePath & _default, FilePath * const _value ) const
    {
        return Detail::hasValueT( m_records, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryConfig::hasValue( const Char * _section, const Char * _key, const Tags & _default, Tags * const _value ) const
    {
        return Detail::hasValueT( m_records, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryConfig::hasValue( const Char * _section, const Char * _key, const Resolution & _default, Resolution * const _value ) const
    {
        return Detail::hasValueT( m_records, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryConfig::hasValue( const Char * _section, const Char * _key, const Color & _default, Color * const _value ) const
    {
        return Detail::hasValueT( m_records, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryConfig::setValue( const Char * _section, const Char * _key, bool _value )
    {
        return Detail::setValueT( m_records, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryConfig::setValue( const Char * _section, const Char * _key, int64_t _value )
    {
        return Detail::setValueT( m_records, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryConfig::setValue( const Char * _section, const Char * _key, double _value )
    {
        return Detail::setValueT( m_records, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryConfig::setValue( const Char * _section, const Char * _key, const String & _value )
    {
        return Detail::setValueT( m_records, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryConfig::setValue( const Char * _section, const Char * _key, const ConstString & _value )
    {
        return Detail::setValueT( m_records, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryConfig::setValue( const Char * _section, const Char * _key, const FilePath & _value )
    {
        return Detail::setValueT( m_records, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryConfig::setValue( const Char * _section, const Char * _key, const Tags & _value )
    {
        return Detail::setValueT( m_records, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryConfig::setValue( const Char * _section, const Char * _key, const Resolution & _value )
    {
        return Detail::setValueT( m_records, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryConfig::setValue( const Char * _section, const Char * _key, const Color & _value )
    {
        return Detail::setValueT( m_records, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    void MemoryConfig::getValues( const Char * _section, const Char * _key, VectorAspectRatioViewports * const _values ) const
    {
        Detail::calcValuesT( m_records, m_platformTags, _section, _key, _values );
    }
    //////////////////////////////////////////////////////////////////////////
    void MemoryConfig::getValues( const Char * _section, const Char * _key, VectorFilePath * const _values ) const
    {
        Detail::calcValuesT( m_records, m_platformTags, _section, _key, _values );
    }
    //////////////////////////////////////////////////////////////////////////
    void MemoryConfig::getValues( const Char * _section, const Char * _key, VectorConstString * const _values ) const
    {
        Detail::calcValuesT( m_records, m_platformTags, _section, _key, _values );
    }
    //////////////////////////////////////////////////////////////////////////
    void MemoryConfig::getValues( const Char * _section, const Char * _key, VectorString * const _values ) const
    {
        Detail::calcValuesT( m_records, m_platformTags, _section, _key, _values );
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
    //////////////////////////////////////////////////////////////////////////
}
