#include "MosaicKeyCodeTable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        class MosaicKeyCodeTable
        {
        public:
            MosaicKeyCodeTable()
            {
                for( uint32_t index = 0; index != MENGINE_INPUT_MAX_KEY_CODE; ++index )
                {
                    m_codes[index] = Mosaic::KeyCode::Unknown;
                }

                m_codes[KC_TAB] = Mosaic::KeyCode::Tab;
                m_codes[KC_RETURN] = Mosaic::KeyCode::Enter;
                m_codes[KC_ESCAPE] = Mosaic::KeyCode::Escape;
                m_codes[KC_SPACE] = Mosaic::KeyCode::Space;
                m_codes[KC_BACK] = Mosaic::KeyCode::Backspace;
                m_codes[KC_DELETE] = Mosaic::KeyCode::Delete;
                m_codes[KC_LEFT] = Mosaic::KeyCode::Left;
                m_codes[KC_RIGHT] = Mosaic::KeyCode::Right;
                m_codes[KC_UP] = Mosaic::KeyCode::Up;
                m_codes[KC_DOWN] = Mosaic::KeyCode::Down;
                m_codes[KC_HOME] = Mosaic::KeyCode::Home;
                m_codes[KC_END] = Mosaic::KeyCode::End;
                m_codes[KC_PRIOR] = Mosaic::KeyCode::PageUp;
                m_codes[KC_NEXT] = Mosaic::KeyCode::PageDown;

                for( uint32_t index = 0; index != 26; ++index )
                {
                    m_codes[KC_A + index] = (Mosaic::KeyCode)((uint32_t)Mosaic::KeyCode::A + index);
                }

                for( uint32_t index = 0; index != 10; ++index )
                {
                    m_codes[KC_0 + index] = (Mosaic::KeyCode)((uint32_t)Mosaic::KeyCode::D0 + index);
                }

                for( uint32_t index = 0; index != 12; ++index )
                {
                    m_codes[KC_F1 + index] = (Mosaic::KeyCode)((uint32_t)Mosaic::KeyCode::F1 + index);
                }
            }

        public:
            Mosaic::KeyCode getKeyCode( EKeyCode _code ) const
            {
                if( (uint32_t)_code >= MENGINE_INPUT_MAX_KEY_CODE )
                {
                    return Mosaic::KeyCode::Unknown;
                }

                Mosaic::KeyCode code = m_codes[(uint32_t)_code];

                return code;
            }

        protected:
            Mosaic::KeyCode m_codes[MENGINE_INPUT_MAX_KEY_CODE];
        };
        //////////////////////////////////////////////////////////////////////////
        static const MosaicKeyCodeTable s_mosaicKeyCodeTable;
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        Mosaic::KeyCode getMosaicKeyCode( EKeyCode _code )
        {
            Mosaic::KeyCode code = Detail::s_mosaicKeyCodeTable.getKeyCode( _code );

            return code;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
}
