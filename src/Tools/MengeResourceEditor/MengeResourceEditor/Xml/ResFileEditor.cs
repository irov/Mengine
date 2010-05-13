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
        XmlNode rootNode;


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
            rootNode = xmlDoc.SelectSingleNode("DataBlock");
            XmlNodeList nodeList = rootNode.SelectNodes("Resource");

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
        public void SaveXml()
        {
            xmlDoc.Save(filePath);
        }

        /// <summary>
        /// 
        /// </summary>
        public void ToResourceImageDefault(XmlNode _node)
        {
            string name = _node.Attributes.GetNamedItem("Name").Value;
            string type = _node.Attributes.GetNamedItem("Type").Value;

            Nodes.ResourceImageDefault imageDefault = new Nodes.ResourceImageDefault(name, type, _node);
            foreach(XmlNode subnode in _node.SelectNodes("File"))
            {
                if(subnode.Attributes.GetNamedItem("NoAtlas") == null){
                    XmlAttribute noAtlas = xmlDoc.CreateAttribute("NoAtlas");
                    noAtlas.Value = "0";
                    subnode.Attributes.Append(noAtlas);
                }
                if (subnode.Attributes.GetNamedItem("NoJPEG") == null)
                {
                    XmlAttribute noJPEG = xmlDoc.CreateAttribute("NoJPEG");
                    noJPEG.Value = "0";
                    subnode.Attributes.Append(noJPEG);
                }
                string path = subnode.Attributes.GetNamedItem("Path").Value;
                imageDefault.addFile(new Nodes.File(path, subnode));
            }

            resImageDefaultList.Add(imageDefault);
        }
        /*
        /// <summary>
        /// 
        /// </summary>
        /// <param name="image"></param>
        public void FromResourceImageDefault(Nodes.ResourceImageDefault _image)
        {
            XmlNode imageNode = CreateResourceNode(_image.Name, _image.Type);

            foreach(Nodes.File file in _image.FileList)
            {
                XmlNode fileNode = xmlDoc.CreateElement("File");
                XmlAttribute filePath = xmlDoc.CreateAttribute("Path");
                filePath.Value = file.Path;
                fileNode.Attributes.Append(filePath);
                if(file.NoAtlas == true){
                    XmlAttribute fileNoAtlas = xmlDoc.CreateAttribute("NoAtlas");
                    fileNoAtlas.Value = "1";
                    fileNode.Attributes.Append(fileNoAtlas);
                }
                if(file.NoJpeg == true){
                    XmlAttribute fileNoJpeg = xmlDoc.CreateAttribute("NoJPEG");
                    fileNoJpeg.Value = "1";
                    fileNode.Attributes.Append(fileNoJpeg);
                }
                imageNode.AppendChild(fileNode);
            }
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="name"></param>
        /// <param name="type"></param>
        public XmlNode CreateResourceNode(string _name, string _type)
        {
            XmlNode xmlNode = xmlDoc.CreateElement("Resource");
            XmlAttribute nodeName = xmlDoc.CreateAttribute("Name");
            nodeName.Value = _name;
            XmlAttribute nodeType = xmlDoc.CreateAttribute("Type");
            nodeType.Value = _type;

            xmlNode.Attributes.Append(nodeName);
            xmlNode.Attributes.Append(nodeType);
            rootNode.AppendChild(xmlNode);

            return xmlNode;
        }
        */
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
