using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Drawing;

namespace MengeTextLocalizer
{
    class CheckedListBoxColored : CheckedListBox
    {
        private Dictionary<int, Brush> m_coloredItems = new Dictionary<int, Brush>();
        public void setItemColor(int _index, Brush _brush)
        {
            if (m_coloredItems.ContainsKey(_index) == false)
            {
                m_coloredItems.Add(_index, _brush);
            }
            else
            {
                m_coloredItems[_index] = _brush;
            }
            this.Invalidate(this.GetItemRectangle(_index));
        }
        
        protected override void OnDrawItem(DrawItemEventArgs e)
        {
            if (e.Index == -1)
            {
                return;
            }
            
            e.DrawBackground();
            System.Windows.Forms.VisualStyles.CheckBoxState visualState = System.Windows.Forms.VisualStyles.CheckBoxState.UncheckedNormal;
            
            if (this.GetItemCheckState(e.Index) == CheckState.Checked)
            {
                visualState = System.Windows.Forms.VisualStyles.CheckBoxState.CheckedNormal;
            }
            System.Windows.Forms.CheckBoxRenderer.DrawCheckBox( e.Graphics, e.Bounds.Location, visualState );

            Brush drawBrush = Brushes.Black;

            if (m_coloredItems.ContainsKey(e.Index))
            {
                drawBrush = m_coloredItems[e.Index];
            }


            if ((e.State & DrawItemState.Selected) != 0)
            {
                drawBrush = Brushes.White;
            }
            string key = (string)this.Items[e.Index];
         // DrawItemEventArgs ne = new DrawItemEventArgs(e.Graphics, e.Font,
           //     new Rectangle(e.Bounds.X + 12, e.Bounds.Y, e.Bounds.Width, e.Bounds.Height),
           //     e.Index, e.State);
            e.Graphics.DrawString(key, e.Font, drawBrush, new Rectangle( e.Bounds.X + 12, e.Bounds.Y, e.Bounds.Width, e.Bounds.Height), StringFormat.GenericDefault);
            //ne.Graphics.DrawString(key, ne.Font, drawBrush, ne.Bounds, StringFormat.GenericDefault);
            //ne.DrawFocusRectangle();
            e.DrawFocusRectangle();
        }
    }
}
