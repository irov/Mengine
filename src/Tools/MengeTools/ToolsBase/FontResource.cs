using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;
using System.Drawing;

namespace MengeToolsBase
{
    public class FontResource
    {
        private struct CharInfo
        {
            public int width;
            public int height;
            public int rectWidth;
            public int rectHeight;
            public int offsetX;
            public int offsetY;
        }

        private Dictionary<char, CharInfo> m_charInfoMap;
         
        public bool LoadFromFile(string _fontdefFilename)
        {
            XmlDocument xmlDoc = new XmlDocument();
            try
            {
                xmlDoc.Load(_fontdefFilename);
            }
            catch (Exception)
            {
                return false;
            }

            m_charInfoMap = new Dictionary<char, CharInfo>();

            XmlNode xmlHeight = xmlDoc.SelectSingleNode("/Font/@height");
            if (xmlHeight == null)
            {
                return false;
            }
            int height = int.Parse(xmlHeight.Value);
            XmlNodeList xmlCharList = xmlDoc.SelectNodes("/Font/Char");
            foreach (XmlNode xmlNode in xmlCharList)
            {
                char code = '\0';
                int width = 0;
                int rectX = 0;
                int rectY = 0;
                int rectW = 0;
                int rectH = 0;
                int offsetX = 0;
                int offsetY = 0;
                foreach (XmlAttribute xmlAttrib in xmlNode.Attributes)
                {
                    if (xmlAttrib.Name == "code")
                    {
                        code = xmlAttrib.Value.ToCharArray()[0];
                    }
                    else if (xmlAttrib.Name == "width")
                    {
                        width = int.Parse(xmlAttrib.Value);
                    }
                    else if (xmlAttrib.Name == "rect")
                    {
                        string[] rect = xmlAttrib.Value.Split(' ');
                        if (rect.Length != 4)
                        {
                            return false;
                        }
                        rectX = int.Parse(rect[0]);
                        rectY = int.Parse(rect[1]);
                        rectW = int.Parse(rect[2]);
                        rectH = int.Parse(rect[3]);
                    }
                    else if (xmlAttrib.Name == "offset")
                    {
                        string[] offset = xmlAttrib.Value.Split(' ');
                        if (offset.Length != 2)
                        {
                            return false;
                        }
                        offsetX = int.Parse(offset[0]);
                        offsetY = int.Parse(offset[1]);
                    }
                }
                   CharInfo charInfo = new CharInfo();
                    charInfo.height = height;
                    charInfo.width = width;
                    charInfo.offsetX = offsetX;
                    charInfo.offsetY = offsetY;
                    charInfo.rectWidth = rectW;
                    charInfo.rectHeight = rectH;
                    m_charInfoMap.Add( code, charInfo );
            }
            
            return true;
        }

        public Size getTextSize(string _text, int _charOffset, int _lineOffset)
        {
            int maxWidth = 0;
            int maxHeight = 0;
            string[] lines = _text.Split('\n');
            foreach (string line in lines)
            {
                int width = 0;
                int maxCharHeight = 0;
                foreach (char ch in line.ToCharArray())
                {
                    if (m_charInfoMap.ContainsKey(ch) == false)
                    {
                        continue;
                    }

                    CharInfo chInfo = m_charInfoMap[ch];
                    width += (chInfo.width + _charOffset);
                    maxCharHeight = Math.Max(maxCharHeight, chInfo.height);
                }
                width -= _charOffset;

                if (line.Length != 0)
                {
                    char firstChar = line[0];
                    char lastChar = line[line.Length - 1];
                    if (m_charInfoMap.ContainsKey(firstChar) == true)
                    {
                        CharInfo chInfo = m_charInfoMap[firstChar];
                        width -= chInfo.offsetX;
                    }
                    if (m_charInfoMap.ContainsKey(lastChar) == true)
                    {
                        CharInfo chInfo = m_charInfoMap[lastChar];
                        width += (chInfo.rectWidth - chInfo.width + chInfo.offsetX);
                    }
                }

                maxWidth = Math.Max(maxWidth, width);

                maxHeight += (maxCharHeight + _lineOffset);
            }
            maxHeight -= _lineOffset;

            if (lines.Length != 0)
            {
                string firstLine = lines[0];
                string lastLine = lines[lines.Length - 1];
                int maxOffset = 0;
                foreach (char ch in firstLine)
                {
                    if (m_charInfoMap.ContainsKey(ch))
                    {
                        CharInfo chInfo = m_charInfoMap[ch];
                        maxOffset = Math.Max(maxOffset, -chInfo.offsetY);
                    }
                }
                maxHeight += maxOffset;
                maxOffset = 0;
                foreach (char ch in lastLine)
                {
                    if (m_charInfoMap.ContainsKey(ch))
                    {
                        CharInfo chInfo = m_charInfoMap[ch];
                        maxOffset = Math.Max(maxOffset, chInfo.rectHeight - chInfo.height + chInfo.offsetY);
                    }
                }
                maxHeight += maxOffset;
            }

            return new Size( maxWidth, maxHeight );
        }
        
        static public FontResource CreateFontResource(string _fontdefFilename)
        {
            FontResource fontResource = new FontResource();
            if (fontResource.LoadFromFile(_fontdefFilename) == false)
            {
                return null;
            }
            return fontResource;
        }
    }

}
