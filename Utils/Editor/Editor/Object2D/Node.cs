using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.Drawing;
using System.Xml;

namespace Editor
{
    public abstract class Node
    {
        public abstract String getName();
        public abstract void setName(String name);
        public abstract void save(ref XmlTextWriter writer);
        public abstract void load(ref ResourceManager resourceManager, XmlNodeList ChildNodes);
        public abstract void setPosition(int x, int y);
        public abstract void setPosX(int x);
        public abstract void setPosY(int y);
        public abstract int getPosX();
        public abstract int getPosY();
        public abstract void translate(int offsetX, int offsetY);
        public abstract void draw(ref Graphics g);
        public abstract void select();
        public abstract void deselect();
        public abstract bool isAligned();
        public abstract bool isHotSpot();

        //
        public abstract Point getLocalPosition();
       // public abstract Point getWorldPosition();

        public virtual Point getWorldPosition()
        {
            if (m_parent == null)
            {
                return ((Node)this).getLocalPosition();
            }

            Point parentWorldPosition = m_parent.getWorldPosition();

            Point worldPos = new Point(parentWorldPosition.X + m_localPosition.X,
                parentWorldPosition.Y + m_localPosition.Y);

            return worldPos;
        }

        protected Node m_parent;

        //List<Node> childrens;
    }
}
