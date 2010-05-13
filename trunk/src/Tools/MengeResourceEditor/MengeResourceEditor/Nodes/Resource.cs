using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;

namespace MengeResourceEditor.Nodes
{
    class Resource: Node
    {
        protected string name;
        protected string type;
        protected XmlAttribute xmlName;
        /// <summary>
        /// 
        /// </summary>
        /// <param name="_name"></param>
        /// <param name="_type"></param>
        public Resource(string _name, string _type, XmlNode _node)
        {
            name = _name;
            type = _type;
            xmlNode = _node;
            xmlName = xmlNode.Attributes.GetNamedItem("Name") as XmlAttribute;
        }

        public Resource()
        {

        }
        /// <summary>
        /// 
        /// </summary>
        public string Name
        {
            get
            {
                return name;
            }
            set
            {
                name = value;
                xmlName.Value = value;
            }
        }
        /// <summary>
        /// 
        /// </summary>
        public string Type
        {
            get
            {
                return type;
            }
        }
        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public override string ToString ()
        {
            return Name;
        }
    }
}
