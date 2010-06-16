using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;
using System.Xml;

namespace MengeResourceEditor.Nodes
{
    class ResourceImageDefault: Resource
    {
        List<File> fileList = new List<File>();
        /// <summary>
        /// 
        /// </summary>
        /// <param name="_name"></param>
        /// <param name="_type"></param>
        public ResourceImageDefault(string _name, string _type, XmlNode _node) : base(_name, _type, _node)
        {
            
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="_file"></param>
        public void addFile(File _file)
        {
            fileList.Add(_file);
        }
        /// <summary>
        /// 
        /// </summary>
        [Browsable(false)]
        public List<File> FileList
        {
            get{
                return fileList;
            }
        }
    }
}
