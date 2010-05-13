using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace MengeResourceEditor
{
    public partial class MainForm : Form
    {
        ProjectEditor projectEditor = new ProjectEditor();
        
        /// <summary>
        /// 
        /// </summary>
        public MainForm()
        {
            InitializeComponent();
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void MainForm_Load(object sender, EventArgs e)
        {
            
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void onFileOpenClick(object sender, EventArgs e)
        {
            FolderBrowserDialog dialog = new FolderBrowserDialog();
            dialog.SelectedPath = Properties.Settings.Default.LastFolderPath;
            if (dialog.ShowDialog() == DialogResult.OK)
            {
                Properties.Settings.Default.LastFolderPath = dialog.SelectedPath;

                projectEditor.Initialize(dialog.SelectedPath);
                UpdatePacksList();

                m_gbGlobalGroup.Enabled = true;
            }
        }
        /// <summary>
        /// 
        /// </summary>
        private void UpdatePacksList()
        {
            m_cbPackName.Items.Clear();

            foreach(PackEditor pack in projectEditor.Packs)
            {
                m_cbPackName.Items.Add(pack);
            }
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void onPackNameChange(object sender, EventArgs e)
        {
            m_cbResourceName.Items.Clear();

            PackEditor pack = m_cbPackName.SelectedItem as PackEditor;
            foreach(ResFileEditor resFile in pack.ResourceFiles)
            {
                m_cbResourceName.Items.Add(resFile);
            }
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void onResourceNameChanged(object sender, EventArgs e)
        {
            ResFileEditor resFile = m_cbResourceName.SelectedItem as ResFileEditor;

            m_twResourceTree.Nodes.Clear();

            foreach(Nodes.ResourceImageDefault res in resFile.ResImageDefaultList)
            {
                MyTreeNode node = new MyTreeNode(res);
                m_twResourceTree.Nodes.Add(node);
            }
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void onResourceTreeSelect(object sender, TreeViewEventArgs e)
        {
            MyTreeNode selectedNode = m_twResourceTree.SelectedNode as MyTreeNode;
            Nodes.Node xmlNode = selectedNode.XmlNode;

            m_pgResourceProperty.SelectedObject = xmlNode;
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void onBtnSaveClick(object sender, EventArgs e)
        {
            projectEditor.SaveXml();
        }
    }
}