using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.Drawing;
using System.Xml;

namespace Editor
{
    interface Node
    {
        Matrix getTransform();
        String getName();
        void setName(String name);
        void save(ref XmlTextWriter writer);
        void load(ref ResourceManager resourceManager, XmlNodeList ChildNodes);
        void setPosition(int x, int y);
        void setPosX(int x);
        void setPosY(int y);
        int  getPosX();
        int  getPosY();
        void move(int offsetX, int offsetY);
        void draw(ref Graphics g);
        void select();
        void deselect();
        bool isAligned();
        bool isHotSpot();
    }
}
