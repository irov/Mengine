using System;
using System.Collections.Generic;
using System.Text;

namespace MengeToolsBase
{
    public class TextNode
    {
        private int m_charOffset;
        private int m_lineOffset;
        private string m_font;
        private string m_key;
        private int m_maxHeight;
        private int m_maxWidth;
        private string m_text;
        private int m_revision;
        private bool m_localized;

        public TextNode(string _key,
            string _text,
            string _font,
            int _maxWidth,
            int _maxHeight,
            int _charOffset,
            int _lineOffset,
            int _revision,
            bool _localized)
        {
            m_key = _key;
            m_text = _text;
            m_font = _font;
            m_maxWidth = _maxWidth;
            m_maxHeight = _maxHeight;
            m_charOffset = _charOffset;
            m_lineOffset = _lineOffset;
            m_revision = _revision;
            m_localized = _localized;
        }

        public TextNode(TextNode _txtNode)
        {
            m_key = _txtNode.m_key;
            m_text = _txtNode.m_text;
            m_font = _txtNode.m_font;
            m_maxWidth = _txtNode.m_maxWidth;
            m_maxHeight = _txtNode.m_maxHeight;
            m_charOffset = _txtNode.m_charOffset;
            m_lineOffset = _txtNode.m_lineOffset;
            m_revision = _txtNode.m_revision;
            m_localized = _txtNode.m_localized;
        }

        public string Text
        {
            get
            {
                return m_text;
            }
            set
            {
                m_text = value;
            }
        }

        public string Font
        {
            get
            {
                return m_font;
            }
            set
            {
                m_font = value;
            }
        }

        public int MaxWidth
        {
            get
            {
                return m_maxWidth;
            }
            set
            {
                m_maxWidth = value;
            }
        }

        public int MaxHeight
        {
            get
            {
                return m_maxHeight;
            }
            set
            {
                m_maxHeight = value;
            }
        }

        public int CharOffset
        {
            get
            {
                return m_charOffset;
            }
            set
            {
                m_charOffset = value;
            }
        }

        public string Key
        {
            get
            {
                return m_key;
            }
            set
            {
                m_key = value;
            }
        }

        public int LineOffset
        {
            get
            {
                return m_lineOffset;
            }
            set
            {
                m_lineOffset = value;
            }
        }

        public int Revision
        {
            get
            {
                return m_revision;
            }
            set
            {
                m_revision = value;
            }
        }

        public bool Localized
        {
            get
            {
                return m_localized;
            }
            set
            {
                m_localized = value;
            }
        }

    }
}
