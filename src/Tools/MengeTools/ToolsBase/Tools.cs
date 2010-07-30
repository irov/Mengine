using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;
using System.Collections;
using System.Windows.Forms;
using System.IO;

namespace MengeToolsBase
{
    public class Tools
    {
        public static string getTextsPathFromResourceXml(XmlDocument _xmlDoc)
        {
            XmlNode xmlTextPath = _xmlDoc.SelectSingleNode("/Resources/Text/@Path");
            if (xmlTextPath == null)
            {
                return null;
            }
            return xmlTextPath.Value;
        }

        public static List<TextNode> parseTextXML(XmlDocument xmlDoc)
        {
            List<TextNode> retList = new List<TextNode>();
            List<string> keysList = new List<string>();
            string font = "";
            int maxWidth = 0;
            int maxHeight = 0;
            int charOffset = 0;
            int lineOffset = 0;
            string key = "";
            string value = "";
            int revision = 0;
            bool translated = false;

            XmlNodeList xmlTextsList = xmlDoc.SelectNodes("/*/Texts");
            foreach (XmlNode xmlNode in xmlTextsList)
            {
                font = "";
                maxWidth = 0;
                maxHeight = 0;
                charOffset = 0;
                lineOffset = 0;
                bool maxSizeOverride = false;
                bool fontOverride = false;
                bool charOffsetOverride = false;
                bool lineOffsetOverride = false;
                foreach (XmlAttribute xmlAttrib in xmlNode.Attributes)
                {
                    if (xmlAttrib.Name == "Font")
                    {
                        font = xmlAttrib.Value;
                        fontOverride = true;
                    }
                    else if (xmlAttrib.Name == "MaxSize")
                    {
                        int idx = xmlAttrib.Value.IndexOf(';');
                        string width = xmlAttrib.Value.Substring(0, idx);
                        string height = xmlAttrib.Value.Substring(idx + 1);
                        maxWidth = int.Parse(width);
                        maxHeight = int.Parse(height);
                        maxSizeOverride = true;
                    }
                    else if (xmlAttrib.Name == "CharOffset")
                    {
                        charOffset = int.Parse(xmlAttrib.Value);
                        charOffsetOverride = true;
                    }
                    else if (xmlAttrib.Name == "LineOffset")
                    {
                        lineOffset = int.Parse(xmlAttrib.Value);
                        lineOffsetOverride = true;
                    }
                }
                foreach (XmlNode xmlSubNode in xmlNode.ChildNodes)
                {
                    if (xmlSubNode.Name == "Text")
                    {
                        key = "";
                        value = "";
                        revision = 0;
                        translated = false;
                        foreach (XmlAttribute xmlAttrib in xmlSubNode.Attributes)
                        {
                            if (xmlAttrib.Name == "Key")
                            {
                                key = xmlAttrib.Value;
                            }
                            else if (xmlAttrib.Name == "Value")
                            {
                                value = xmlAttrib.Value;
                            }
                            else if (fontOverride == false && xmlAttrib.Name == "Font")
                            {
                                font = xmlAttrib.Value;
                            }
                            else if (maxSizeOverride == false && xmlAttrib.Name == "MaxSize")
                            {
                                int idx = xmlAttrib.Value.IndexOf(';');
                                string width = xmlAttrib.Value.Substring(0, idx);
                                string height = xmlAttrib.Value.Substring(idx + 1);
                                maxWidth = int.Parse(width);
                                maxHeight = int.Parse(height);
                            }
                            else if (charOffsetOverride == false && xmlAttrib.Name == "CharOffset")
                            {
                                charOffset = int.Parse(xmlAttrib.Value);
                            }
                            else if (lineOffsetOverride == false && xmlAttrib.Name == "LineOffset")
                            {
                                lineOffset = int.Parse(xmlAttrib.Value);
                            }
                            else if (xmlAttrib.Name == "Revision")
                            {
                                revision = int.Parse(xmlAttrib.Value);
                            }
                            else if (xmlAttrib.Name == "Translated")
                            {
                                translated = int.Parse(xmlAttrib.Value) != 0;
                            }
                        }
                        TextNode node = new TextNode(key, value, font, maxWidth, maxHeight, charOffset, lineOffset, revision, translated);
                        retList.Add(node);
                        keysList.Add(key);
                    }
                }
            }

            xmlTextsList = xmlDoc.SelectNodes("/*/Text");
            foreach (XmlNode xmlNode in xmlTextsList)
            {
                font = "";
                maxWidth = 0;
                maxHeight = 0;
                charOffset = 0;
                lineOffset = 0;
                key = "";
                value = "";
                revision = 0;
                translated = false;
                foreach (XmlAttribute xmlAttrib in xmlNode.Attributes)
                {

                    if (xmlAttrib.Name == "Key")
                    {
                        key = xmlAttrib.Value;
                    }
                    else if (xmlAttrib.Name == "Value")
                    {
                        value = xmlAttrib.Value;
                    }
                    else if (xmlAttrib.Name == "Font")
                    {
                        font = xmlAttrib.Value;
                    }
                    else if (xmlAttrib.Name == "MaxSize")
                    {
                        int idx = xmlAttrib.Value.IndexOf(';');
                        string width = xmlAttrib.Value.Substring(0, idx);
                        string height = xmlAttrib.Value.Substring(idx + 1);
                        maxWidth = int.Parse(width);
                        maxHeight = int.Parse(height);
                    }
                    else if (xmlAttrib.Name == "CharOffset")
                    {
                        charOffset = int.Parse(xmlAttrib.Value);
                    }
                    else if (xmlAttrib.Name == "LineOffset")
                    {
                        lineOffset = int.Parse(xmlAttrib.Value);
                    }
                    else if (xmlAttrib.Name == "Revision")
                    {
                        revision = int.Parse(xmlAttrib.Value);
                    }
                    else if (xmlAttrib.Name == "Translated")
                    {
                        translated = int.Parse(xmlAttrib.Value) != 0;
                    }
                }
                if (keysList.Contains(key) == true)
                {
                    MessageBox.Show("Error: text with key \"" + key + "\" already exist\nWarning: Duplicate entry removed", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
                else
                {
                    TextNode node = new TextNode(key, value, font, maxWidth, maxHeight, charOffset, lineOffset, revision, translated);
                    retList.Add(node);
                    keysList.Add(key);
                }
            }
            return retList;
        }

        public static Dictionary<string, FontResource> LoadFonts(XmlDocument _xmlDoc, string _rootFolder)
        {
            Dictionary<string, FontResource> retMap = new Dictionary<string, FontResource>();
            ArrayList resourceFileList = new ArrayList();
            XmlNodeList xmlResourceList = _xmlDoc.SelectNodes("/Resources/Resource");
            foreach (XmlNode xmlResourceNode in xmlResourceList)
            {
                string path = "";
                foreach (XmlAttribute xmlAttrib in xmlResourceNode.Attributes)
                {
                    if (xmlAttrib.Name == "Path")
                    {
                        path = xmlAttrib.Value;
                    }
                }
                XmlNodeList xmlResourceNames = xmlResourceNode.SelectNodes("Resource/@Name");
                foreach (XmlNode xmlResourceName in xmlResourceNames)
                {
                    string resFilename = _rootFolder;
                    resFilename += System.IO.Path.DirectorySeparatorChar;
                    resFilename += path;
                    resFilename += System.IO.Path.DirectorySeparatorChar;
                    resFilename += xmlResourceName.Value + ".resource";
                    resourceFileList.Add(resFilename);
                }

            }

            XmlDocument xmlDoc = new XmlDocument();
            foreach (string filename in resourceFileList)
            {
                try
                {
                    xmlDoc.Load(filename);
                }
                catch (Exception exp)
                {
                    MessageBox.Show("Can't load resource file \'" + filename + "\' :" + exp.Message, "Error",
                        MessageBoxButtons.OK, MessageBoxIcon.Error);
                    continue;
                }

                XmlNodeList xmlResList = xmlDoc.GetElementsByTagName("Resource");
                foreach (XmlNode xmlNode in xmlResList)
                {
                    XmlNode typeNode = xmlNode.Attributes.GetNamedItem("Type");
                    XmlNode nameNode = xmlNode.Attributes.GetNamedItem("Name");
                    if (typeNode == null || typeNode.Value != "ResourceFont"
                        || nameNode == null)
                    {
                        continue;
                    }

                    string name = nameNode.Value;

                    string fontdef = "";
                    // string imagepath = "";
                    foreach (XmlNode xmlSubNode in xmlNode.ChildNodes)
                    {
                        if (xmlSubNode.Name == "Fontdef")
                        {
                            XmlNode pathNode = xmlSubNode.Attributes.GetNamedItem("Path");
                            if (pathNode == null) continue;
                            fontdef = pathNode.Value;
                        }
                        //else if (xmlSubNode.Name == "Image")
                        //{
                        //    XmlNode pathNode = xmlSubNode.Attributes.GetNamedItem("Path");
                        //    if (pathNode == null) continue;
                        //    imagepath = pathNode.Value;
                        //}
                    }

                    FontResource fontResource = FontResource.CreateFontResource(fontdef);
                    if (fontResource != null)
                    {
                        retMap.Add(name, fontResource);
                    }
                }
            }

            return retMap;
        }

        public static XmlDocument toXMLDocument(List<TextNode> _textNodeList)
        {
            XmlDocument xmlDoc = new XmlDocument();
            XmlNode root = xmlDoc.CreateElement("TextsData");

            foreach (TextNode txtNode in _textNodeList)
            {
                XmlNode node = xmlDoc.CreateElement("Text");
                XmlAttribute key = xmlDoc.CreateAttribute("Key");
                XmlAttribute value = xmlDoc.CreateAttribute("Value");
                XmlAttribute font = xmlDoc.CreateAttribute("Font");
                XmlAttribute maxSize = xmlDoc.CreateAttribute("MaxSize");
                XmlAttribute charOffset = xmlDoc.CreateAttribute("CharOffset");
                XmlAttribute lineOffset = xmlDoc.CreateAttribute("LineOffset");
                XmlAttribute revision = xmlDoc.CreateAttribute("Revision");

                key.Value = txtNode.Key;
                value.Value = txtNode.Text;
                font.Value = txtNode.Font;
                maxSize.Value = txtNode.MaxWidth.ToString() + ';' + txtNode.MaxHeight.ToString();
                charOffset.Value = txtNode.CharOffset.ToString();
                lineOffset.Value = txtNode.LineOffset.ToString();
                revision.Value = txtNode.Revision.ToString();

                node.Attributes.Append(key);
                node.Attributes.Append(value);
                node.Attributes.Append(font);
                node.Attributes.Append(maxSize);
                node.Attributes.Append(charOffset);
                node.Attributes.Append(lineOffset);

                if (txtNode.Localized == true)
                {
                    XmlAttribute localized = xmlDoc.CreateAttribute("Localized");
                    localized.Value = "1";
                }

                root.AppendChild(node);
            }
            xmlDoc.AppendChild(root);
            return xmlDoc;
        }

        private static Predicate<TextNode> textNodeKeyMatch(string _key)
        {
            return delegate(TextNode _textNode)
            {
                return _textNode.Key == _key;
            };
        }

        private static List<string> ReadCSVLine(StreamReader fs)
        {
            List<string> al = new List<string>();
            int ch = fs.Read();
            int colDelim = 0; // possible values ',', ';', '\t', '|'
            int quoteChar;

            if (ch == -1)
                return al;
            while (ch != -1 && ch == '\r' || ch == '\n' || ch == ' ')
                ch = fs.Read();

            if (ch == -1)
                return al;

            while (ch != -1 && ch != '\r' && ch != '\n')
            {
                string sb = "";
                if (ch == '\'' || ch == '"')
                {
                    quoteChar = ch;
                    int c = fs.Read();
                    bool done = false;
                    while (!done && c != -1)
                    {
                        while (c != -1 && c != ch)
                        {
                            sb += (char)c;
                            c = fs.Read();
                        }
                        if (c == ch)
                        {
                            done = true;
                            int next = fs.Read(); // consume end quote
                            if (next == ch)
                            {
                                // it was an escaped quote sequence "" inside the literal
                                // so append a single " and consume the second end quote.
                                done = false;
                                sb += (char)next;
                                c = fs.Read();
                                if (colDelim != 0 && c == colDelim)
                                {
                                    // bad form, but this is probably a record separator.
                                    done = true;
                                }
                            }
                            else if (colDelim != 0 && next != colDelim && next != -1 &&
                                     next != ' ' && next != '\n' && next != '\r')
                            {
                                // it was an un-escaped quote embedded inside a string literal
                                // in this case the quote is probably just part of the text so ignore it.
                                done = false;
                                sb += c;
                                sb += next;
                                c = fs.Read();
                            }
                            else
                            {
                                c = next;
                            }
                        }
                    }
                    ch = c;
                }
                else
                {
                    // scan number, date, time, float, etc.
                    while (ch != -1 && ch != '\n' && ch != '\r')
                    {
                        if (ch == colDelim ||
                            (colDelim == 0 &&
                             (ch == ',' || ch == ';' || ch == '\t' || ch == '|')))
                            break;
                        sb += ch;
                        ch = fs.Read();
                    }
                }
                al.Add(sb);
                if (ch == colDelim ||
                    (colDelim == 0 && (ch == ',' || ch == ';' || ch == '\t' || ch == '|')))
                {
                    colDelim = ch;
                    ch = fs.Read();
                    if (ch == '\n' || ch == '\r')
                    {
                        al.Add(""); // blank field.
                    }
                }
            }
            return al;
        }

        public static void loadCVS(string _filename, List<TextNode> _textNodeList)
        {
            try
            {
                StreamReader stream = new StreamReader(_filename, true);
                string line = stream.ReadLine();
                char separator = line[1];
                string[] cells;
                string key;
                string text;

                List<string> row = ReadCSVLine(stream);
                while (row.Count > 0)
                {
                    if (row.Count > 1)
                    {
                        key = row[0];
                        text = row[1];
                        TextNode nodeToUpdate = _textNodeList.Find(textNodeKeyMatch(key));
                        if (nodeToUpdate != null)
                        {
                            nodeToUpdate.Text = text;
                        }
                    }
                    row = ReadCSVLine(stream);
                }
            }
            catch (Exception e)
            {
                MessageBox.Show("Invalid file format (" + e.Message + ")");
            }
        }

        public static void saveCVS(string _filename, List<TextNode> _textNodeList)
        {
            try
            {
                StreamWriter stream = new StreamWriter(_filename, false, Encoding.UTF8);
                stream.WriteLine(" ;");
                string nextLine;
                string cell;
                foreach (TextNode txtNode in _textNodeList)
                {
                    nextLine = txtNode.Key.Replace("\"", "\"\"");
                    nextLine = "\"" + nextLine + "\"";
                    cell = txtNode.Text.Replace("\"", "\"\"");
                    cell = "\"" + cell + "\"";
                    nextLine = nextLine + ';' + cell;
                    stream.WriteLine(nextLine);
                }
                stream.Flush();
                stream.Close();
            }
            catch (Exception e)
            {
                MessageBox.Show("Error while saving file");
            }
        }
    }
}
