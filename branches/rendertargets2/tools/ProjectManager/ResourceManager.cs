using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;
using System.IO;

namespace ProjectManager
{
    public class ResourceManager
    {
        public void AddResource(ResourceImageDefault resource)
        {
            resources.Add(resource.GetName(), resource);
        }

        public int GetCount()
        {
            return resources.Count;
        }

        public ResourceImageDefault GetResource(String name)
        {
            ResourceImageDefault result = null;
            resources.TryGetValue(name, out result);
            return result;
        }

        public Dictionary<String, ResourceImageDefault>.KeyCollection GetKeys()
        {
            return resources.Keys;
        }

        public bool loadResource(String fileName)
        {
            XmlDocument xDoc = new XmlDocument();

            xDoc.Load(fileName);

            String offsetPath = Path.GetDirectoryName(fileName);
            //SPECIFIC TO MENGE. 
            offsetPath = Path.GetDirectoryName(offsetPath);

            XmlNodeList list = xDoc.GetElementsByTagName("Resource");

            foreach (XmlNode node in list)
            {
                XmlAttributeCollection attr = node.Attributes;

                String nodeName = attr.Item(0).Value;
                String nodeType = attr.Item(1).Value;

                if (nodeType == "ResourceImageDefault")
                {
                    ResourceImageDefault resource = new ResourceImageDefault(nodeName, nodeType);

                    foreach (XmlNode childNode in node.ChildNodes)
                    {
                        XmlNodeType nType = childNode.NodeType;

                        if (nType == XmlNodeType.Comment)
                        {
                            continue;
                        }

                        String path = childNode.Attributes.Item(0).Value;

                        String filePath = offsetPath + "\\" + path;

                        if (File.Exists(filePath))
                        {
                            resource.AddFile(filePath);
                        }
                    }

                    AddResource(resource);
                }
            }
            return true;
        }

        private Dictionary<String, ResourceImageDefault> resources = new Dictionary<String, ResourceImageDefault>();
    }
}
