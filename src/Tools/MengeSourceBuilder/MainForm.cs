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
        private Dictionary<string, string> m_tools = new Dictionary<string, string>();
        private List<Job> m_jobs = new List<Job>();
        private Job m_activeJob = null;

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

        private Package parsePackage_(XmlNode _xmlNode, string _baseDir)
        {
            Package pkg = new Package();
            XmlNode xmlType = _xmlNode.SelectSingleNode("@type");
            if (xmlType != null)
            {
                string type = xmlType.Value.ToLower();
                if (type == "svn")
                {
                    pkg.m_type = PackageType.Svn;
                }
                else if (type == "archive")
                {
                    pkg.m_type = PackageType.Archive;
                }
            }
            XmlNode xmlUrl = _xmlNode.SelectSingleNode("@url");
            if (xmlUrl != null)
            {
                pkg.m_url = xmlUrl.Value;
            }
            XmlNode xmlPath = _xmlNode.SelectSingleNode("@path");
            if (xmlPath != null)
            {
                pkg.m_path = Path.Combine( _baseDir, xmlPath.Value.Replace( '/', Path.DirectorySeparatorChar ) );
            }
            return pkg;
        }

        private void onUpdateClick(object sender, EventArgs e)
        {
            if (m_editDestFolder.Text.Length == 0)
            {
                MessageBox.Show("Destination folder is empty. Specify destination folder, please");
                return;
            }

            DirectoryInfo destFolder = new DirectoryInfo(m_editDestFolder.Text);
            if (destFolder.Exists == false)
            {
                if (MessageBox.Show("Destination folder does not exists. Create and procceed?", "", MessageBoxButtons.OKCancel) ==
                    DialogResult.Cancel)
                {
                    return;
                }
                try
                {
                    destFolder.Create();
                }
                catch (Exception)
                {
                    MessageBox.Show("Failed to create destination folder", "Error");
                    return;
                }
            }

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

            m_tools.Clear();
            XmlNodeList nodeList = xmlConfig.SelectNodes("/SourceBuilderConfig/Tool");
            foreach (XmlNode xmlNode in nodeList)
            {
                try
                {
                    XmlNode xmlType = xmlNode.SelectSingleNode("@type");
                    XmlNode xmlPath = xmlNode.SelectSingleNode("@path");
                    m_tools.Add(xmlType.Value.ToLower(), xmlPath.Value);
                }
                catch (Exception)
                {
                    // don't care
                }
            }
            m_packages.Clear();
            nodeList = xmlConfig.SelectNodes("/SourceBuilderConfig/Sources/Package");
            foreach (XmlNode xmlNode in nodeList)
            {
                Package pkg = parsePackage_(xmlNode, destFolder.FullName);
                m_packages.Add(pkg);
            }
            m_btnUpdate.Hide();
            m_btnCancel.Show();
            m_editDestFolder.Enabled = false;
            m_btnBrowse.Enabled = false;
            m_progressAll.Show();
            m_btnCancel.Focus();
            doJobs_();
        }

        private void doJobs_()
        {
            foreach (Package pkg in m_packages)
            {
                if (pkg.m_type == PackageType.Svn)
                {
                    if (m_tools.ContainsKey("svn") == true)
                    {
                        m_jobs.Add(new JobSVNExport( Path.Combine( Directory.GetCurrentDirectory(), m_tools["svn"] ), pkg.m_url, pkg.m_path));
                    }
                }
                else if (pkg.m_type == PackageType.Archive)
                {
                    if (m_tools.ContainsKey("archive") == true)
                    {
                        m_jobs.Add(new JobArchive( Path.Combine( Directory.GetCurrentDirectory(), m_tools["archive"] ), pkg.m_url, pkg.m_path));
                    }
                }
            }
            m_progressAll.Maximum = m_jobs.Count;
            m_jobsTimer.Start();
            runNextJob_();
        }

        private void runNextJob_()
        {
            if (m_jobs.Count > 0)
            {
                m_activeJob = m_jobs[0];
                m_jobs.RemoveAt(0);
                m_activeJob.run();
            }
            else
            {
                onJobsEnd_();
            }
        }

        private void onJobExited_()
        {
            m_progressAll.Increment(1);
            runNextJob_();
        }

        private void onJobsEnd_()
        {
            m_jobsTimer.Stop();
            m_btnCancel.Hide();
            m_btnUpdate.Show();
            m_progressAll.Hide();
            m_editDestFolder.Enabled = true;
            m_btnBrowse.Enabled = true;
        }

        private void onCancelClick(object sender, EventArgs e)
        {
            if (m_activeJob != null)
            {
                m_activeJob.stop();
            }
            onJobsEnd_();
        }

        private void onFormClosed(object sender, FormClosedEventArgs e)
        {
            if (m_activeJob != null)
            {
                m_activeJob.stop();
            }
        }

        private void onJobsUpdateEvent_(object sender, EventArgs e)
        {
            if (m_activeJob != null && m_activeJob.IsRunning == false)
            {
                onJobExited_();
            }
        }

    }
}