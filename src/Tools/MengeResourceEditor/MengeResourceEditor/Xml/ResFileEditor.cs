using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;

namespace MengeResourceEditor
{
    class ResFileEditor
    {
        delegate void AddMethod(XmlNode _node);

        string name;
        string filePath;
        XmlDocument xmlDoc = new XmlDocument();
        List<Nodes.ResourceImageDefault> resImageDefaultList = new List<Nodes.ResourceImageDefault>();

        Dictionary<string, AddMethod> addMethodDict = new Dictionary<string, AddMethod>();
        

        /// <summary>
        /// 
        /// </summary>
        /// <param name="_name"></param>
        /// <param name="_filePath"></param>
        public ResFileEditor(string _name, string _filePath)
        {
            addMethodDict.Add("ResourceImageDefault", ToResourceImageDefault);
            addMethodDict.Add("ResourceFont", ToResourceFont);
            addMethodDict.Add("ResourceEmitterContainer", ToResourceEmitterContainer);
            addMethodDict.Add("ResourcePlaylist", ToResourcePlaylist);
            addMethodDict.Add("ResourceAnimation", ToResourceAnimation);

            name = _name;
            filePath = _filePath;

            UpdateFromXml();
        }
        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public override string ToString()
        {
            return name;
        }
        /// <summary>
        /// 
        /// </summary>
        public void UpdateFromXml()
        {
            xmlDoc.Load(filePath);
            XmlNodeList nodeList = xmlDoc.SelectSingleNode("DataBlock").SelectNodes("Resource");

            foreach(XmlNode node in nodeList)
            {
                try
                {
                    string type = node.Attributes.GetNamedItem("Type").Value;
                    addMethodDict[type](node);
                }
                catch(Exception e)
                {

                }
            }
        }
        
        /// <summary>
        /// 
        /// </summary>
        public void ToResourceImageDefault(XmlNode _node)
        {
            string name = _node.Attributes.GetNamedItem("Name").Value;
            string type = _node.Attributes.GetNamedItem("Type").Value;

            Nodes.ResourceImageDefault imageDefault = new Nodes.ResourceImageDefault(name, type);
            foreach(XmlNode subnode in _node.SelectNodes("File"))
            {
                string path = subnode.Attributes.GetNamedItem("Path").Value;
                imageDefault.addFile(new Nodes.File(path));
            }

            resImageDefaultList.Add(imageDefault);
        }
        /// <summary>
        /// 
        /// </summary>
        public List<Nodes.ResourceImageDefault> ResImageDefaultList
        {
            get{
                return resImageDefaultList;
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="_node"></param>
        public void ToResourceFont(XmlNode _node)
        {

        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="_node"></param>
        public void ToResourceEmitterContainer(XmlNode _node)
        {

        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="_node"></param>
        public void ToResourcePlaylist(XmlNode _node)
        {

        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="_node"></param>
        public void ToResourceAnimation(XmlNode _node)
        {

        }

    }
}
