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
    class Layer : Node
    {
        public List<Node> Nodes = new List<Node>();

        public Layer(String _name)
        {
            LayerName = _name;
        }

        public override Point getLocalPosition()
        {
            return m_localPosition;                        
        }

        public override Point getWorldPosition()
        {
            return m_localPosition;
        }

        public override String getName()
        {
            return LayerName;
        }

        public override void setName(String name)
        {
            LayerName = name;
        }

        public override void save(ref XmlTextWriter writer)
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
            writer.WriteAttributeString("Value", Size.X.ToString() + ";" + Size.Y.ToString());
            writer.WriteEndElement();

            foreach (Node node in Nodes)
            {
                node.save(ref writer);
            }

            writer.WriteEndElement();
        }

        public override void load(ref ResourceManager resourceManager, XmlNodeList ChildNodes)
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

        public override void setPosition(int x, int y)
        {
            m_localPosition.X = x;
            m_localPosition.Y = y;
        }

        public override void setPosX(int x)
        {
            m_localPosition.X = x;
        }

        public override void setPosY(int y)
        {
            m_localPosition.Y = y;
        }

        public override int getPosX()
        {
            return m_localPosition.X;
        }

        public override int getPosY()
        {
            return m_localPosition.Y;
        }

        public override void translate(int offsetX, int offsetY)
        {
            foreach (Node node in Nodes)
            {
                node.translate(offsetX, offsetY);
            }
        }

        public override void draw(ref Graphics g)
        {
            foreach (Node node in Nodes)
            {
                node.draw(ref g);
            }
        }

        public override void select()
        {

        }

        public override void deselect()
        {

        }

        public override bool isAligned()
        {
            return false;
        }

        public override bool isHotSpot()
        {
            return false;
        }

        public void SwapNodes(String prevNodeName, String nodeName)
        {
            int left = Nodes.FindIndex(delegate(Node desc) { return desc.getName() == prevNodeName; });
            int right = Nodes.FindIndex(delegate(Node desc) { return desc.getName() == nodeName; });

            object swap = Nodes[left];
            Nodes[left] = Nodes[right];
            Nodes[right] = (Node)swap;
        }

        public void SetMain(bool _isMain)
        {
            isMain = _isMain;
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
            return m_localPosition;
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

        public void RemoveNode(String name)
        {
            Node item = Nodes.Find(delegate(Node desc) { return desc.getName() == name; });
            Nodes.Remove(item);
        }

        private bool isMain = false;
        private bool isScrollable = false;
        private String LayerName = "";
        private Point Size = new Point(0,0);

        private Point m_localPosition = new Point(0, 0);
    }
}
