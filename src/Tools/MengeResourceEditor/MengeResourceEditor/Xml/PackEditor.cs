using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;

namespace MengeResourceEditor
{
    class PackEditor
    {
        string name;
        string fullPath;
        string resFileName;
        
        List<ResFileEditor> resFiles = new List<ResFileEditor>();

        /// <summary>
        /// 
        /// </summary>
        /// <param name="_name"></param>
        /// <param name="_fullPath"></param>
        /// <param name="_resFileName"></param>
        public PackEditor(string _name, string _fullPath, string _resFileName)
        {
            name = _name;
            fullPath = _fullPath;
            resFileName = _resFileName;

            try
            {
                UpdateFromXml();
            }
            catch(Exception e)
            {
                System.Windows.Forms.MessageBox.Show(e.Message);
            }
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
            string filePath = fullPath + System.IO.Path.DirectorySeparatorChar + resFileName;
            XmlDocument doc = new XmlDocument();
            doc.Load(filePath);

            XmlNode resource = doc.FirstChild.SelectSingleNode("Resource");
            string resourcePath = resource.Attributes.GetNamedItem("Path").Value;

            foreach(XmlNode node in resource.SelectNodes("Resource"))
            {
                string resName = node.Attributes.GetNamedItem("Name").Value;
                string fullFilePath = fullPath + System.IO.Path.DirectorySeparatorChar + resourcePath + System.IO.Path.DirectorySeparatorChar + resName + ".resource";
                ResFileEditor resourceFile = new ResFileEditor(resName, fullFilePath);

                resFiles.Add(resourceFile);
            }
        }
        /// <summary>
        /// 
        /// </summary>
        public List<ResFileEditor> ResourceFiles
        {
            get
            {
                return resFiles;
            }
        }
    }
}
