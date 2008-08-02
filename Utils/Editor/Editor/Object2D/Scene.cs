using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Xml;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.Drawing;


namespace Editor
{
    class Scene
    {
       private List<Layer> Layers = new List<Layer>();

        public void Draw(Graphics graph, BufferedGraphics bufferedGraphics)
        {
            graph.Clear(Color.CornflowerBlue);

            foreach (Node layer in Layers)
            {
                layer.draw(ref graph);
            }

            bufferedGraphics.Render();
        }

        public void FillTreeView(ref TreeView tree)
        {
            foreach (Node layer in Layers)
            {
                tree.Nodes.Add(layer.getName());
            }
        }

        public void Remove(Layer item)
        {
            Layers.Remove(item);
        }
       
        public Layer AddLayer(String name)
        {
            Layer layer = new Layer(name);
            layer.SetMain(false);
            layer.SetScroll(false);
            Layers.Add(layer);
            return layer;
        }

        public Layer GetLayer(String layerName)
        {
            Layer layer = Layers.Find(delegate(Layer desc) { return ((Node)desc).getName() == layerName; });
            return layer;
        }

        public void SwapLayers(String prevNodeName, String nodeName)
        {
            int left = Layers.FindIndex(delegate(Layer desc) { return ((Node)desc).getName() == prevNodeName; });
            int right = Layers.FindIndex(delegate(Layer desc) { return ((Node)desc).getName() == nodeName; });

            object swap = Layers[left];
            Layers[left] = Layers[right];
            Layers[right] = (Layer)swap;
        }

        public void Save()
       {
           SaveFileDialog fileDialog = new SaveFileDialog();

           fileDialog.Title = "Scene Files";
           fileDialog.InitialDirectory = @"";
           fileDialog.Filter = "Scene Files (*.xml)|*.xml";
           fileDialog.FilterIndex = 0;
           fileDialog.RestoreDirectory = true;

           if (fileDialog.ShowDialog() == DialogResult.OK)
           {
               XmlTextWriter writer = new XmlTextWriter(fileDialog.FileName, Encoding.Unicode);
               writer.Indentation = 2;
               writer.IndentChar = ' ';
               writer.Formatting = System.Xml.Formatting.Indented;

               writer.WriteStartDocument();
               writer.WriteStartElement("Scene", "");


               foreach (Layer layer in Layers)
               {
                   ((Node)layer).save(ref writer);
               }

               writer.WriteEndElement();
               writer.WriteEndDocument();

               writer.Flush();
               writer.Close();
           }
       }

       public void Load(ref ResourceManager resourceManager)
       {
           OpenFileDialog fileDialog = new OpenFileDialog();

           fileDialog.Title = "Scene Files";
           fileDialog.InitialDirectory = @"";
           fileDialog.Filter = "Scene Files (*.xml)|*.xml";
           fileDialog.FilterIndex = 0;
           fileDialog.RestoreDirectory = true;

           if (fileDialog.ShowDialog() == DialogResult.OK)
           {

               Layers.Clear();

               XmlDocument doc = new XmlDocument();
               doc.Load(fileDialog.FileName);

               XmlNodeList list = doc.GetElementsByTagName("Scene");
               XmlNode n = list.Item(0);

               //для всех лайеров
               foreach (XmlNode nodeLayer in n.ChildNodes)
               {
                   String Name = nodeLayer.Attributes.Item(0).Value;
                   String Type = nodeLayer.Attributes.Item(1).Value;

                   if (Type == "Layer2D")
                   {
                       Layer layer = new Layer(Name);
                       ((Node)layer).load(ref resourceManager, nodeLayer.ChildNodes);

                       Layers.Add(layer);
                   }
               }
           }
       }
    }
}
