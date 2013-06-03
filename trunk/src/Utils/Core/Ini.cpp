#   include "Ini.h"

#   include "Interface/UnicodeInterface.h"

#   include "Logger/Logger.h"

#   include <stdio.h>

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    static void s_rtrim( char * s )
    {
        size_t len = strlen(s);

        char * e = s + len - 1;

        while( *e == ' ' || *e == '\t' )
        {
            --e;
        }

        *(e + 1) = '\0';
    }
    //////////////////////////////////////////////////////////////////////////
    static char * s_trim( char * s )
    {
        while( *s == ' ' || *s == '\t' )
        {
            ++s;
        }

        s_rtrim( s );

        return s;
    } 
    //////////////////////////////////////////////////////////////////////////
    Ini::Ini( ServiceProviderInterface * _serviceProvider )
        : m_serviceProvider(_serviceProvider)
        , m_currentSection(nullptr)
        , m_settingsCount(0)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Ini::load( const InputStreamInterfacePtr & _input )
    {
        size_t size = _input->size();

        if( size >= ini_buff_size )
        {
            LOGGER_ERROR(m_serviceProvider)("Ini::load ini size %d max %d"
                , size
                , ini_buff_size
                );

            return false;
        }

        _input->read( m_buff, size );
        m_buff[size] = '\0';

        m_settingsCount = 0;
                
        for( char * line = strtok( m_buff, "\r\n" ); line; line = strtok( nullptr, "\r\n" ) )
        {
            if( this->loadLine_( line ) == false )
            {
                return false;
            }
        }
              
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Ini::loadLine_( char * _line )
    {
        char * trim_line = s_trim( _line );

        size_t len = strlen( trim_line );

        if( len == 0 )
        {
            return true;
        }
        
        if( trim_line[0] == ';' )
        {
            return true;
        }

        if( trim_line[0] == '[' && trim_line[len-1] == ']' )
        {   
            char name[64];
            if( sscanf( trim_line, "[%[^]]", name ) != 1 )
            {
                return false;
            }

            char * section_str = strstr( trim_line, name );
            size_t section_len = strlen( name );

            section_str[section_len] = '\0';

            m_currentSection = section_str;

            return true;
        }

        if( m_currentSection == nullptr )
        {
            return false;
        }

        char key[64];
        char value[256];              
        if( sscanf( trim_line, "%[^=] = \"%[^\"]\"", key, value ) == 2
            ||  sscanf( trim_line, "%[^=] = '%[^\']'", key, value ) == 2
            ||  sscanf( trim_line, "%[^=] = %[^;#]", key, value ) == 2 )         
        {            
            char * key_str = strstr( trim_line, key );
            size_t key_len = strlen( key );
            key_str[key_len] = '\0';
            s_rtrim( key_str );

            char * value_str = strstr( trim_line + key_len + 1, value );
            size_t value_len = strlen( value );
            value_str[value_len] = '\0';
            s_rtrim( value_str );

            Setting & setting = m_settings[m_settingsCount];

            setting.section = m_currentSection;
            setting.key = key_str;
            setting.value = value_str;

            ++m_settingsCount;

            return true;
        }

        if( sscanf( trim_line, "%[^=] =", key ) == 1 )
        {
            char * key_str = strstr( trim_line, key );
            size_t key_len = strlen( key );
            key_str[key_len] = '\0';
            s_rtrim( key_str );

            Setting & setting = m_settings[m_settingsCount];

            setting.section = m_currentSection;
            setting.key = key_str;
            setting.value = key_str + key_len;

            ++m_settingsCount;

            return true;
        }

        return false;
    }    
    //////////////////////////////////////////////////////////////////////////
    const char * Ini::getSettingValue( const char * _section, const char * _key ) const
    {
        for( size_t index = 0; index != m_settingsCount; ++index )
        {
            const Setting & setting = m_settings[index];

            if( strcmp( setting.section, _section ) != 0 )
            {
                continue;
            }

            if( strcmp( setting.key, _key ) != 0 )
            {
                continue;
            }

            const char * value = setting.value;

            return value;
        }

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Ini::countSettingValues( const char * _section, const char * _key ) const
    {
        size_t count = 0;

        for( size_t index = 0; index != m_settingsCount; ++index )
        {
            const Setting & setting = m_settings[index];

            if( strcmp( setting.section, _section ) != 0 )
            {
                continue;
            }

            if( strcmp( setting.key, _key ) != 0 )
            {
                continue;
            }

            ++count;
        }

        return count;
    }
    //////////////////////////////////////////////////////////////////////////
    const char * Ini::getSettingValues( const char * _section, const char * _key, size_t _index ) const
    {
        size_t count = 0;

        for( size_t index = 0; index != m_settingsCount; ++index )
        {
            const Setting & setting = m_settings[index];

            if( strcmp( setting.section, _section ) != 0 )
            {
                continue;
            }

            if( strcmp( setting.key, _key ) != 0 )
            {
                continue;
            }
            
            if( count != _index )
            {
                ++count;

                continue;
            }
             
            const char * value = setting.value;

            return value;
        }

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Ini::countSettings( const char * _section ) const
    {
        size_t count = 0;

        for( size_t index = 0; index != m_settingsCount; ++index )
        {
            const Setting & setting = m_settings[index];

            if( strcmp( setting.section, _section ) != 0 )
            {
                continue;
            }

            ++count;
        }

        return count;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Ini::getSettings( const char * _section, size_t _index, const char ** _key, const char ** _value ) const
    {
        size_t count = 0;

        for( size_t index = 0; index != m_settingsCount; ++index )
        {
            const Setting & setting = m_settings[index];

            if( strcmp( setting.section, _section ) != 0 )
            {
                continue;
            }
            
            if( count != _index )
            {
                ++count;

                continue;
            }
            
            *_key = setting.key;
            *_value = setting.value;

            return true;
        }

        return false;
    }
}