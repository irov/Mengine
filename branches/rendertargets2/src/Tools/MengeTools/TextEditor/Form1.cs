using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Xml;
using System.Collections;
using MengeToolsBase;

namespace MengeTextEditor
{
    public partial class Form1 : Form
    {
        private List<TextNode> m_textNodeOrigMap;
        private Dictionary<string, FontResource> m_fontResourcesMap;

        private string m_filenameOriginal;
        private string m_resourceFilename;
        private TextBox m_listEditBox = new TextBox();
        private int m_selectedItemIndex = -1;
        private int m_textHandleCounter = 0;
        private bool m_documentModified = false;
        private bool m_readyForCommit = false;
        private string m_findNextString = null;
        private bool m_findNextMatchCase = false;
        private string m_findPosition = null;
        private FindNext m_findNextForm = null;
        private TreeNode sourceNode;

        public Form1()
        {
            InitializeComponent();
        }

        private TextNode getTextNodeByKey(string _key)
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

        private void toolStripMenuItemEdit_Click(object sender, EventArgs e)
        {
            if (m_openFileDialog.ShowDialog() != DialogResult.OK)
            {
                return;
            }

            m_resourceFilename = m_openFileDialog.FileName;
            XmlDocument xmlDocRes = new XmlDocument();
            try
            {
                xmlDocRes.Load(m_resourceFilename);
            }
            catch (Exception exp)
            {
                MessageBox.Show("Failed to open xml file: " + exp.Message,
                    "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            string root = System.IO.Directory.GetParent(m_resourceFilename).FullName;
            System.IO.Directory.SetCurrentDirectory( root );
            m_filenameOriginal = Tools.getTextsPathFromResourceXml(xmlDocRes);
            if (m_filenameOriginal == null)
            {
                MessageBox.Show("Text resources not found", "Error");
                return;
            }
            m_readyForCommit = false;

            XmlDocument xmlDoc = new XmlDocument();
            try
            {
                xmlDoc.Load(m_filenameOriginal);
            }
            catch (Exception exp)
            {
                MessageBox.Show("Failed to open xml file: " + exp.Message,
                    "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            m_textHandleCounter = 0;
            m_textNodeOrigMap = Tools.parseTextXML(xmlDoc);
            m_documentModified = false;

            string folder = System.IO.Path.GetDirectoryName(m_resourceFilename);
            m_fontResourcesMap = Tools.LoadFonts(xmlDocRes, folder);

            m_fontCombo.Items.Clear();
            foreach (KeyValuePair<string, FontResource> fontResourceMapEntry in m_fontResourcesMap)
            {
                m_fontCombo.Items.Add(fontResourceMapEntry.Key);
            }

            foreach (TextNode txtNode in m_textNodeOrigMap)
            {
                //itemList.Items.Add(txtNode.Key);
                TreeNode treeNode = m_treeView.Nodes.Add(txtNode.Key);

                Color color = Color.Red;
                if (m_fontResourcesMap.ContainsKey(txtNode.Font) == true)
                {
                    Size size = m_fontResourcesMap[txtNode.Font].getTextSize(txtNode.Text,
                        txtNode.CharOffset, txtNode.LineOffset);
                    if (size.Width <= txtNode.MaxWidth
                        && size.Height <= txtNode.MaxHeight)
                    {
                        color = Color.Black;
                    }
                }
                treeNode.ForeColor = color;
            }

            m_readyForCommit = true;
        }

        private bool renameKey(string _oldKey, string _newKey)
        {
            if (getTextNodeByKey(_newKey) != null)
            {
                return false;
            }

            TextNode txtNode = getTextNodeByKey(_oldKey);
            txtNode.Key = _newKey;
            m_documentModified = true;
            return true;
        }

        private void m_btnPlus_Click(object sender, EventArgs e)
        {
            string newKey = "NewTextHandle" + m_textHandleCounter.ToString();
            m_textHandleCounter += 1;
            while (getTextNodeByKey(newKey) != null)
            {
                newKey = "NewTextHandle" + m_textHandleCounter.ToString();
                m_textHandleCounter += 1;
            }
            string fontName = "";
            foreach (string font in m_fontResourcesMap.Keys)
            {
                fontName = font;
                break;
            }
            TextNode newTextNode = new TextNode(newKey, "", fontName, 0, 0, 0, 0, 0, false);
            if (m_selectedItemIndex != -1)
            {
                TextNode selTextNode = m_textNodeOrigMap[m_selectedItemIndex];
                newTextNode.Font = selTextNode.Font;
                newTextNode.MaxWidth = selTextNode.MaxWidth;
                newTextNode.MaxHeight = selTextNode.MaxHeight;
                newTextNode.CharOffset = selTextNode.CharOffset;
                newTextNode.LineOffset = selTextNode.LineOffset;
            }

            m_textNodeOrigMap.Insert(m_selectedItemIndex + 1, newTextNode);
            m_treeView.SelectedNode = m_treeView.Nodes.Insert(m_selectedItemIndex + 1, newKey);
            //itemList.Items.Insert(m_selectedItemIndex+1, newKey);
            //itemList.SelectedIndex = m_selectedItemIndex + 1;
            m_documentModified = true;
        }

        private void m_btnMinus_Click(object sender, EventArgs e)
        {
            if (m_selectedItemIndex == -1)
            {
                return;
            }
            m_textNodeOrigMap.RemoveAt(m_selectedItemIndex);
            m_treeView.Nodes.RemoveAt(m_selectedItemIndex);
            m_documentModified = true;
        }

        private void selectTextNode(int _index)
        {
            TextNode txtNode = m_textNodeOrigMap[_index];
            //int sel = m_editOriginal.SelectionStart;
            m_editOriginal.Text = txtNode.Text;// txtNode.Text.Split('\n');
            // m_editOriginal.SelectionStart = sel;
            m_fontCombo.Text = txtNode.Font;
            m_maxWidthNumeric.Value = txtNode.MaxWidth;
            m_maxHeightNumeric.Value = txtNode.MaxHeight;
            m_lineOffsetNumeric.Value = txtNode.LineOffset;
            m_charOffsetNumeric.Value = txtNode.CharOffset;

            Color color = Color.Red;
            if (m_fontResourcesMap.ContainsKey(txtNode.Font) == true)
            {
                FontResource fontResource = m_fontResourcesMap[txtNode.Font];
                Size txtSize = fontResource.getTextSize(txtNode.Text, txtNode.CharOffset, txtNode.LineOffset);
                m_actualSizeEdit.Text = txtSize.ToString();
                if (txtSize.Width <= txtNode.MaxWidth
                    && txtSize.Height <= txtNode.MaxHeight)
                {
                    color = Color.Black;
                }
            }
            m_editOriginal.ForeColor = color;
        }

        private void editTextNode_Leave(object sender, EventArgs e)
        {
            commitChange();
        }

        private void save(string _filename)
        {
            XmlDocument xmlDoc = Tools.toXMLDocument(m_textNodeOrigMap);
            xmlDoc.Save(_filename);
            m_documentModified = false;
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (m_filenameOriginal == null || m_filenameOriginal == "")
            {
                return;
            }

            save(m_filenameOriginal);
        }

        private void toolStripMenuItem1_Click(object sender, EventArgs e)
        {
            //itemList.Select();
            m_treeView.Select();
            if (m_saveFileDialog.ShowDialog() != DialogResult.OK)
            {
                return;
            }

            m_filenameOriginal = m_saveFileDialog.FileName;
            save(m_filenameOriginal);
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            e.Cancel = false;
            if (m_filenameOriginal == null || m_filenameOriginal == ""
                || m_documentModified == false)
            {
                return;
            }

            DialogResult result = MessageBox.Show("Save document before exit?", "Saving", MessageBoxButtons.YesNoCancel, MessageBoxIcon.Question);
            if (result == DialogResult.OK)
            {
                save(m_filenameOriginal);
            }
            else if (result == DialogResult.Cancel)
            {
                e.Cancel = true;
            }
        }

        private void commitChange()
        {
            if (m_readyForCommit == true)
            {
                if (m_selectedItemIndex == -1)
                {
                    return;
                }
                m_readyForCommit = false;
                TextNode txtNode = m_textNodeOrigMap[m_selectedItemIndex];
                txtNode.Text = m_editOriginal.Text.Replace("\r", "");
                txtNode.Font = m_fontCombo.Text;
                txtNode.MaxWidth = (int)m_maxWidthNumeric.Value;
                txtNode.MaxHeight = (int)m_maxHeightNumeric.Value;
                txtNode.CharOffset = (int)m_charOffsetNumeric.Value;
                txtNode.LineOffset = (int)m_lineOffsetNumeric.Value;
                txtNode.Revision += 1;
                m_documentModified = true;
                selectTextNode(m_selectedItemIndex);    // update selection
                m_readyForCommit = true;
            }
        }

        private void m_exitMenuItem_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void m_editOriginal_TextChanged(object sender, EventArgs e)
        {
            if (m_selectedItemIndex == -1)
            {
                return;
            }

            Color color = Color.Red;

            TextNode txtNode = m_textNodeOrigMap[m_selectedItemIndex];
            if (m_fontResourcesMap.ContainsKey(txtNode.Font) == true)
            {
                Size size = m_fontResourcesMap[txtNode.Font].getTextSize(m_editOriginal.Text,
                    txtNode.CharOffset, txtNode.LineOffset);
                m_actualSizeEdit.Text = size.ToString();
                if (size.Width <= txtNode.MaxWidth
                    && size.Height <= txtNode.MaxHeight)
                {
                    color = Color.Black;
                }
            }
            m_editOriginal.ForeColor = color;
            m_treeView.Nodes[m_selectedItemIndex].ForeColor = color;
            txtNode.Text = m_editOriginal.Text;
        }

        private void m_findNextMenuItem_Click(object sender, EventArgs e)
        {
            if (m_findNextForm == null)
            {
                m_findNextForm = new FindNext(this.FindNext);
            }
            if (m_findNextForm.Visible == false)
            {
                m_findNextForm.Show(this);
            }
            else
            {
                m_findNextForm.Activate();
            }
        }

        private void FindNext(string _findMessage, bool matchCase)
        {
            m_findNextString = _findMessage;
            m_findNextMatchCase = matchCase;
            StringComparison scmp = StringComparison.OrdinalIgnoreCase;
            if (m_findNextMatchCase == true)
            {
                scmp = StringComparison.Ordinal;
            }
            bool found = false;
            bool end = false;
            if (m_findPosition == null)
            {
                end = true;
            }
            int index = -1;
            foreach (TextNode txtNode in m_textNodeOrigMap)
            {
                index += 1;
                if (m_findPosition != null)
                {
                    if (m_findPosition == txtNode.Key)
                    {
                        m_findPosition = null;
                    }
                    continue;
                }
                if (txtNode.Text.IndexOf(_findMessage, scmp) != -1
                    || txtNode.Key.IndexOf(_findMessage, scmp) != -1)
                {
                    m_findPosition = txtNode.Key;
                    m_treeView.SelectedNode = m_treeView.Nodes[index];
                    found = true;
                    break;
                }
            }
            if (found == false && end == false)
            {
                m_findPosition = null;
                FindNext(_findMessage, matchCase);
            }
        }

        private void m_treeView_AfterSelect(object sender, TreeViewEventArgs e)
        {
            m_selectedItemIndex = m_treeView.SelectedNode.Index;
            if (m_selectedItemIndex != -1)
            {
                m_readyForCommit = false;
                selectTextNode(m_selectedItemIndex);
                m_readyForCommit = true;
            }
        }

        private void m_treeView_NodeMouseDoubleClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            e.Node.BeginEdit();
        }

        private void m_treeView_AfterLabelEdit(object sender, NodeLabelEditEventArgs e)
        {
            if (e.CancelEdit == false && e.Label != null )
            {
                e.CancelEdit = renameKey(m_textNodeOrigMap[e.Node.Index].Key, e.Label) == false;
            }
        }

        private void onExportToCSVMenuItem(object sender, EventArgs e)
        {
            if (m_exportDialog.ShowDialog() != DialogResult.OK)
            {
                return;
            }

            Tools.saveCVS(m_exportDialog.FileName, m_textNodeOrigMap);

        }

        private void m_treeView_DragOver(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(DataFormats.Text))
            {
                e.Effect = DragDropEffects.Move;
            }
            else
            {
                e.Effect = DragDropEffects.None;
            }
        }

        private void m_treeView_DragDrop(object sender, DragEventArgs e)
        {

            Point pos = m_treeView.PointToClient(new Point(e.X, e.Y));
            TreeNode targetNode = m_treeView.GetNodeAt(pos);
            TreeNode nodeCopy;

            if (targetNode != null)
            {
                nodeCopy = new TreeNode(sourceNode.Text, sourceNode.ImageIndex, sourceNode.SelectedImageIndex);
                TextNode txtSource = m_textNodeOrigMap[sourceNode.Index];
                int targetIndex = targetNode.Index;
                int index = targetIndex + 1;
                if (sourceNode.Index > targetNode.Index)
                {
                    index -= 1;
                }
                m_textNodeOrigMap.Insert(index, new TextNode(txtSource));
                m_treeView.Nodes.Insert(index, nodeCopy);
                m_textNodeOrigMap.RemoveAt(sourceNode.Index);
                sourceNode.Remove();
                m_treeView.SelectedNode = m_treeView.Nodes[targetIndex];
                m_treeView.Invalidate();
            }
        }

        private void m_treeView_ItemDrag(object sender, ItemDragEventArgs e)
        {
            sourceNode = (TreeNode)e.Item;
            m_treeView.SelectedNode = sourceNode;
            DoDragDrop(e.Item.ToString(), DragDropEffects.Move | DragDropEffects.Copy);
        }

        private void onImportFromCSVMenuItem(object sender, EventArgs e)
        {
            if (m_importDialog.ShowDialog() != DialogResult.OK)
            {
                return;
            }

            Tools.loadCVS(m_importDialog.FileName, m_textNodeOrigMap);
        }

    }
}