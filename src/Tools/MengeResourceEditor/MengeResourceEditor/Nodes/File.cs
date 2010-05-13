using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;

namespace MengeResourceEditor.Nodes
{
    class File: Node
    {
        string path;
        bool noAtlas = false;
        bool noJpeg = false;
        protected XmlAttribute xmlPath;
        protected XmlAttribute xmlNoAtlas;
        protected XmlAttribute xmlNoJPEG;
        /// <summary>
        /// 
        /// </summary>
        /// <param name="_path"></param>
        public File(string _path, XmlNode _node)
        {
            path = _path;
            xmlNode = _node;
            xmlPath = _node.Attributes.GetNamedItem("Path") as XmlAttribute;
            xmlNoAtlas = _node.Attributes.GetNamedItem("NoAtlas") as XmlAttribute;
            xmlNoJPEG = _node.Attributes.GetNamedItem("NoJPEG") as XmlAttribute;
        }
        /// <summary>
        /// 
        /// </summary>
        public string Path
        {
            get{
                return path;
            }
            set{
                path = value;
                xmlPath.Value = value;
            }
        }
        /// <summary>
        /// 
        /// </summary>
        public bool NoAtlas
        {
            get{
                return noAtlas;
            }
            set{
                noAtlas = value;
                if (value == true)
                    xmlNoAtlas.Value = "1";
                else
                    xmlNoAtlas.Value = "0";
            }
        }
        /// <summary>
        /// 
        /// </summary>
        public bool NoJpeg
        {
            get{
                return noJpeg;
            }
            set{
                noJpeg = value;
                if (value == true)
                    xmlNoJPEG.Value = "1";
                else
                    xmlNoJPEG.Value = "0";
            }
        }
        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public override string ToString ()
        {
            return Path;
        }
    }
}
