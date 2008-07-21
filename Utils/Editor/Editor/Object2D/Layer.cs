using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.Drawing;
using System.Windows.Forms;
using System.ComponentModel;

using System.Data;
using System.Xml;


namespace Editor
{
    class Layer
    {
        public List<Node> Nodes = new List<Node>();

        public Layer(String _name)
        {
            LayerName = _name;
        }

        public void SwapNodes(String prevNodeName, String nodeName)
        {
            int left = Nodes.FindIndex(delegate(Node desc) { return desc.getName() == prevNodeName; });
            int right = Nodes.FindIndex(delegate(Node desc) { return desc.getName() == nodeName; });

            object swap = Nodes[left];
            Nodes[left] = Nodes[right];
            Nodes[right] = (Node)swap;
        }

        public void SaveXML(ref XmlTextWriter writer)
        {
            writer.WriteStartElement("Node");
            writer.WriteAttributeString("Name", LayerName);
            writer.WriteAttributeString("Type", "Layer2D");

            if (isMain)
            {
                writer.WriteStartElement("Main");
                writer.WriteAttributeString("Value", "1");
                writer.WriteEndElement();
            }

            writer.WriteStartElement("Scrollable");
            writer.WriteAttributeString("Value", (isScrollable == true) ? "1" : "0");
            writer.WriteEndElement();

            writer.WriteStartElement("Size");
            writer.WriteAttributeString("Value", Size.X.ToString() + ";"+Size.Y.ToString());
            writer.WriteEndElement();

            foreach (Node node in Nodes)
            {
                node.save(ref writer);
            }

            writer.WriteEndElement();
        }

        public void LoadXML(ref ResourceManager resourceManager, XmlNodeList ChildNodes)
        {
            foreach (XmlNode node in ChildNodes)
            {
                if (node.NodeType == XmlNodeType.Comment)
                {
                    continue;
                }

                if (node.Attributes.Count == 1)
                {
                    String Value = node.Attributes.Item(0).Value;

                    if (node.Name == "Main")
                    {
                        SetMain(Convert.ToInt32(Value) == 1);
                        continue;
                    }

                    if (node.Name == "Size")
                    {
                        String[] coords = Value.Split(';');
                        SetSize(new Point(Convert.ToInt32(coords[0]), Convert.ToInt32(coords[1])));
                        continue;
                    }

                    if (node.Name == "Scrollable")
                    {
                        SetScroll(Convert.ToInt32(Value) == 1);
                        continue;
                    }
                }

                if (node.Attributes.Count == 2)
                {
                    String Name = node.Attributes.Item(0).Value;
                    String Type = node.Attributes.Item(1).Value;

                    if (Type == "Sprite")
                    {
                        Sprite sprite = new Sprite(Name);
                        ((Node)sprite).load(ref resourceManager, node.ChildNodes);
                        AddNode(sprite);
                        continue;
                    }

                    if (Type == "HotSpot")
                    {
                        HotSpot hotspot = new HotSpot(Name, true);
                        ((Node)hotspot).load(ref resourceManager, node.ChildNodes);
                        AddNode(hotspot);
                        continue;
                    }
                }
            }
        }


        public void SetMain(bool _isMain)
        {
            isMain = _isMain;
        }

        public void SetPosition(int x, int y)
        {
            Position.X = x;
            Position.Y = y;
        }

        public void SetSize(Point size)
        {
            Size = size;       
        }

        public void SetSizeX(int X)
        {
            Size.X = X;
        }

        public void SetSizeY(int Y)
        {
            Size.Y = Y;
        }

        public Point GetSize()
        {
            return Size;
        }

        public Point GetPosition()
        {
            return Position;
        }

        public bool GetMain()
        {
            return isMain;
        }

        public bool IsScroll()
        {
            return isScrollable;
        }

        public void SetScroll(bool scroll)
        {
            isScrollable = scroll;
        }

        public void SetName(String _name)
        {
            LayerName = _name;
        }

        public String GetName()
        {
            return LayerName;
        }

        public Node GetNode(String _name)
        {
            return Nodes.Find(delegate(Node desc) { return desc.getName() == _name; });
        }

        public void AddHotSpot(String name, int X, int Y)
        {
            Node node = new HotSpot(name);
            node.setPosition(X, Y);
            Nodes.Add(node);
        }

        public void AddSprite(String name, ResourceImageDefault resource, int X, int Y)
        {
            Node node = new Sprite(name);
            node.setPosition(X, Y);

            Sprite sprite = (Sprite)node;
            sprite.setResource(ref resource);

            Nodes.Add(node);
        }

        public void AddNode(Node node)
        {
            Nodes.Add(node);
        }

        public void Draw(ref Graphics g)
        {
            foreach (Node node in Nodes)
            {
                node.draw(ref g);
            }
        }

        public void RemoveNode(String name)
        {
            Node item = Nodes.Find(delegate(Node desc) { return desc.getName() == name; });
            Nodes.Remove(item);
        }

        private bool isMain = false;
        private bool isScrollable = false;
        private Point Position = new Point(0, 0);
        private String LayerName = "";
        private Point Size = new Point(0,0);
    }
}
