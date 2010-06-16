using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using MengeToolsBase;
using System.Xml;

namespace MengeTextLocalizer
{
    public partial class Form1 : Form
    {
        private List<TextNode> m_textNodeOrigMap;
        private Dictionary<string, FontResource> m_fontResourcesOrigMap;
        private string m_originalTextsFilename;

        private List<TextNode> m_textNodeLocalMap;
        private Dictionary<string, FontResource> m_fontResourcesLocalMap;
        private string m_localTextsFilename;
        private int m_done = 0;
        private int m_all = 0;

        private int m_selectedItemIndex = -1;

        public Form1()
        {
            InitializeComponent();
        }

        private TextNode getOrigTextNodeByKey(string _key)
        {
            foreach (TextNode txtNode in m_textNodeOrigMap)
            {
                if (_key == txtNode.Key)
                {
                    return txtNode;
                }
            }
            return null;
        }

        private void m_exitMainMenuItem_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            if (m_openOriginal.ShowDialog() != DialogResult.OK)
            {
                this.Close();
                return;
            }

            string resourceOriginal = m_openOriginal.FileName;
            Properties.Settings.Default.s_initDirOriginal = System.IO.Path.GetDirectoryName(resourceOriginal);

            XmlDocument xmlDoc = new XmlDocument();
            try
            {
                xmlDoc.Load(resourceOriginal);
            }
            catch (Exception exp)
            {
                MessageBox.Show("Failed to open xml file: " + exp.Message,
                    "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            m_originalTextsFilename = Tools.getTextsPathFromResourceXml(xmlDoc);
            if (m_originalTextsFilename == null)
            {
                MessageBox.Show("Text resources not found", "Error");
                return;
            }

            string folder = System.IO.Path.GetDirectoryName(resourceOriginal);
            m_fontResourcesOrigMap = Tools.LoadFonts(xmlDoc, folder);

            try
            {
                xmlDoc.Load(m_originalTextsFilename);
            }
            catch (Exception exp)
            {
                MessageBox.Show("Failed to open xml file: " + exp.Message,
                    "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            m_textNodeOrigMap = Tools.parseTextXML(xmlDoc);

            foreach (TextNode txtNode in m_textNodeOrigMap)
            {
                m_itemList.Items.Add(txtNode.Key);
            }
        }

        private void updateEditColor(int _index)
        {
            if (_index == -1)
            {
                return;
            }

            TextNode txtNode = m_textNodeLocalMap[_index];
            Color color = Color.Red;
            Brush brush = Brushes.Red;
            if (m_itemList.GetItemChecked(_index) == true)
            {
                brush = Brushes.Green;
            }
            else if (m_fontResourcesLocalMap.ContainsKey(txtNode.Font) == true)
            {
                Size size = m_fontResourcesLocalMap[txtNode.Font].getTextSize(txtNode.Text,
                    txtNode.CharOffset, txtNode.LineOffset);
                if (size.Width <= txtNode.MaxWidth
                    && size.Height <= txtNode.MaxHeight)
                {
                    color = Color.Black;
                    brush = Brushes.Black;
                }

            }
            m_editTranslation.ForeColor = color;
            m_itemList.setItemColor(_index, brush);

        }

        private void selectItem(int _index)
        {
            
            if (_index == -1)
            {
                return;
            }
            
            TextNode txtNode = m_textNodeOrigMap[_index];
            m_editOriginal.Text = txtNode.Text;
            m_maxSizeEdit.Text = new Size(txtNode.MaxWidth, txtNode.MaxHeight).ToString();

            if (m_localTextsFilename != null)
            {
                TextNode txtNodeLocal = m_textNodeLocalMap[_index];
                m_editTranslation.Text = txtNodeLocal.Text;
                m_fontCombo.Text = txtNodeLocal.Font;
                m_charOffsetNumeric.Value = txtNodeLocal.CharOffset;
                m_lineOffsetNumeric.Value = txtNodeLocal.LineOffset;
                string actSizeStr = "";
                if (m_fontResourcesLocalMap.ContainsKey(txtNodeLocal.Font) == true)
                {
                    actSizeStr = m_fontResourcesLocalMap[txtNodeLocal.Font].getTextSize(txtNodeLocal.Text, txtNodeLocal.CharOffset, txtNodeLocal.LineOffset).ToString();
                }
                m_actualSizeEdit.Text = actSizeStr;
                updateEditColor(_index);
            }
        }

        private void m_itemList_SelectedIndexChanged(object sender, EventArgs e)
        {
            m_selectedItemIndex = m_itemList.SelectedIndex;
            selectItem(m_selectedItemIndex);
        }

        private void m_itemList_ItemCheck(object sender, ItemCheckEventArgs e)
        {
            if (m_localTextsFilename == null)
            {
                e.NewValue = CheckState.Unchecked;
            }
            else
            {
                TextNode origNode = m_textNodeOrigMap[m_selectedItemIndex];
                TextNode localNode = m_textNodeLocalMap[m_selectedItemIndex];
                localNode.Revision = origNode.Revision;
                localNode.Localized = e.NewValue == CheckState.Checked;
                if (e.NewValue == CheckState.Checked)
                {
                    e.NewValue = CheckState.Unchecked;
                    if (m_fontResourcesLocalMap.ContainsKey(localNode.Font) == true)
                    {
                        Size size = m_fontResourcesLocalMap[localNode.Font].getTextSize(localNode.Text,
                            localNode.CharOffset, localNode.LineOffset);
                        if (size.Width <= localNode.MaxWidth
                            && size.Height <= localNode.MaxHeight)
                        {
                            m_itemList.setItemColor(e.Index, Brushes.Green);
                            m_done += 1;
                            e.NewValue = CheckState.Checked;
                        }
                    }
                }
                else
                {
                    //m_itemList.setItemColor(e.Index, Brushes.Black);
                    updateEditColor(e.Index);
                    m_done -= 1;
                }
                m_doneEdit.Text = m_done.ToString() + " / " + m_all.ToString();
            }
        }

        private void m_openLocalizationMainMenuItem_Click(object sender, EventArgs e)
        {
            if (m_openFileLocal.ShowDialog() != DialogResult.OK)
            {
                return;
            }

            string resourceLocal = m_openFileLocal.FileName;
            Properties.Settings.Default.s_initDirLocal = System.IO.Path.GetDirectoryName(resourceLocal);
            XmlDocument xmlDoc = new XmlDocument();
            try
            {
                xmlDoc.Load(resourceLocal);
            }
            catch (Exception exp)
            {
                MessageBox.Show("Failed to open xml file: " + exp.Message,
                    "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            m_localTextsFilename = Tools.getTextsPathFromResourceXml(xmlDoc);
            if (m_localTextsFilename == null)
            {
                MessageBox.Show("Text resources not found", "Error");
                return;
            }

            string folder = System.IO.Path.GetDirectoryName(resourceLocal);
            m_fontResourcesLocalMap = Tools.LoadFonts(xmlDoc, folder);

            try
            {
                xmlDoc.Load(m_localTextsFilename);
            }
            catch (Exception exp)
            {
                MessageBox.Show("Failed to open xml file: " + exp.Message,
                    "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                m_localTextsFilename = null;
                return;
            }

            m_all = 0;
            m_textNodeLocalMap = Tools.parseTextXML(xmlDoc);

            foreach (TextNode txtNodeOrig in m_textNodeOrigMap)
            {
                if (getOrigTextNodeByKey(txtNodeOrig.Key) == null)
                {
                    m_textNodeLocalMap.Add(new TextNode(txtNodeOrig));
                }
            }

            List<TextNode> nodesToDelete = new List<TextNode>(m_textNodeLocalMap.Count);
            foreach ( TextNode txtNode in m_textNodeLocalMap )
            {
                TextNode origNode = getOrigTextNodeByKey(txtNode.Key);
                if (origNode == null)
                {
                    nodesToDelete.Add(txtNode);
                }
                else
                {
                    m_all += 1;
                    if (txtNode.Localized == true
                        && txtNode.Revision == origNode.Revision)
                    {
                        int index = m_itemList.Items.IndexOf( txtNode.Key );
                        m_itemList.SetItemChecked(index, true);
                        m_done += 1;
                    }
                }
            }

            foreach (TextNode node in nodesToDelete)
            {
                m_textNodeLocalMap.Remove(node);
            }

            m_doneEdit.Text = m_done.ToString() + " / " + m_all.ToString();

            foreach (string font in m_fontResourcesLocalMap.Keys)
            {
                m_fontCombo.Items.Add(font);
            }

            for (int index = 0; index < m_itemList.Items.Count; ++index)
            {
                updateEditColor(index);
            }
        }

        private void m_editTranslation_TextChanged(object sender, EventArgs e)
        {
            if (m_selectedItemIndex == -1 || m_localTextsFilename == null)
            {
                return;
            }

            TextNode txtNode = m_textNodeLocalMap[m_selectedItemIndex];
            txtNode.Text = m_editTranslation.Text.Replace("\r","");
            updateEditColor(m_selectedItemIndex);
        }

        private void m_fontCombo_SelectionChangeCommitted(object sender, EventArgs e)
        {
            if (m_selectedItemIndex == -1 || m_localTextsFilename == null)
            {
                return;
            }
            TextNode txtNode = m_textNodeLocalMap[m_selectedItemIndex];
            txtNode.Font = m_fontCombo.Text;
            updateEditColor(m_selectedItemIndex);
       }

        private void m_charOffsetNumeric_ValueChanged(object sender, EventArgs e)
        {
            if (m_selectedItemIndex == -1 || m_localTextsFilename == null)
            {
                return;
            }
            
            TextNode txtNode = m_textNodeLocalMap[m_selectedItemIndex];
            txtNode.CharOffset = (int)m_charOffsetNumeric.Value;
            updateEditColor(m_selectedItemIndex);

        }

        private void m_lineOffsetNumeric_ValueChanged(object sender, EventArgs e)
        {
            if (m_selectedItemIndex == -1 || m_localTextsFilename == null)
            {
                return;
            }
            
            TextNode txtNode = m_textNodeLocalMap[m_selectedItemIndex];
            txtNode.LineOffset = (int)m_lineOffsetNumeric.Value;
            updateEditColor(m_selectedItemIndex);
        }

        private void save(string _filename)
        {
            XmlDocument xmlDoc = Tools.toXMLDocument(m_textNodeLocalMap);
            xmlDoc.Save(_filename);
        }

        private void m_saveLocalizationMainMenuItem_Click(object sender, EventArgs e)
        {
            if (m_localTextsFilename != null)
            {
                save(m_localTextsFilename);
            }
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (m_localTextsFilename == null || m_localTextsFilename == "")
            {
                return;
            }

            DialogResult result = MessageBox.Show("Save document before exit?", "Saving", MessageBoxButtons.YesNoCancel, MessageBoxIcon.Question);
            if (result == DialogResult.OK)
            {
                save(m_localTextsFilename);
            }
            else if (result == DialogResult.Cancel)
            {
                e.Cancel = true;
            }

        }
    }
}