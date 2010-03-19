using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Xml;

namespace MengeSourceBuilder
{
    public partial class MainForm : Form
    {
        private List<Package> m_packages = new List<Package>();

        public MainForm()
        {
            InitializeComponent();
        }

        private void onBrowseClick(object sender, EventArgs e)
        {
            if (m_dlgDestFolder.ShowDialog() == DialogResult.OK)
            {
                m_editDestFolder.Text = m_dlgDestFolder.SelectedPath;
            }
        }

        private void onUpdateClick(object sender, EventArgs e)
        {
            XmlDocument xmlConfig = new XmlDocument();
            try
            {
                xmlConfig.Load("SourceBuilderConfig.xml");
            }
            catch (Exception exception)
            {
                MessageBox.Show("Failed to open configuration file (" + exception.Message + ")", "Error");
                return;
            }

            m_packages.Clear();

        }
    }
}