using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Xml;
using System.IO;

namespace ProjectManager
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
            ProjectManager.Instance.InitProjectManagerDir();
        }

        private void newToolStripMenuItem_Click(object sender, EventArgs e)
        {
            String[] templateNames = ProjectManager.Instance.GetAvaibleTemplates();

            using (NewProjectForm newProject = new NewProjectForm(templateNames))
            {
                if (newProject.ShowDialog() == DialogResult.OK)
                {
                    ProjectManager.Instance.CreateNewProject(newProject.newProjectName, newProject.newProjectLocation, newProject.templateName);

                    GameProperties gameProperties = ProjectManager.Instance.CurrentProject.GetGamePropertiesInfo();

                    gamePropertyGrid.SelectedObject = gameProperties;
                }
            }
        }

        private void runGameToolStripMenuItem_Click(object sender, EventArgs e)
        {
            using (SettingsForm settingsProject = new SettingsForm())
            {
                String args = "";

                if (settingsProject.ShowDialog() == DialogResult.OK)
                {
                    if(settingsProject.sound)
                    {
                        args += "-sound ";
                    }

                    if (settingsProject.debuginfo)
                    {
                        args += "-debuginfo ";
                    }

                    if (settingsProject.particles)
                    {
                        args += "-particles";
                    }

                    ProjectManager.Instance.RunCurrentProject(args);
                }
            }
        }

        public void PopulateTree(string parentDir, TreeNode node)
        {
            DirectoryInfo directory = new DirectoryInfo(parentDir);
         
            foreach (DirectoryInfo dir in directory.GetDirectories())
            {
                if (dir.Name == ".svn")
                {
                    continue;
                }

                TreeNode nodeDir = new TreeNode(dir.Name);
                PopulateTree(dir.FullName, nodeDir);
                node.Nodes.Add(nodeDir);
            }
           
            foreach (FileInfo file in directory.GetFiles())
            {
                if (file.Name == "Thumbs.db")
                {
                    continue;
                }

                TreeNode nodeFile = new TreeNode(file.Name);
                node.Nodes.Add(nodeFile);
            }
        }

        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            DialogResult result = openFileDialog.ShowDialog();

            resourceTreeView.Nodes.Clear();

            if (result == DialogResult.OK)
            {
             /*   ApplicationNet applicationNet = new ApplicationNet();
                bool result1 = applicationNet.initialize(openFileDialog.FileName,"", false);

                GameProperties gameProperties = applicationNet.getGameProperties();
                gamePropertyGrid.SelectedObject = gameProperties;
                */
                //resourceTreeView.Nodes.Add("Bin");
                TreeNode parent = resourceTreeView.Nodes.Add("Project");

                String GameFolder = System.IO.Path.GetDirectoryName(System.IO.Path.GetDirectoryName(openFileDialog.FileName));

                PopulateTree(GameFolder, parent);
                parent.Expand();

                //EnumerateDirectory(parent);

                //GameNet gameNet = new GameNet();
                //string[] resourcePaths = gameNet.getResourcePaths();
                //gameNet.init();
                //gameNet.loadPersonality();

                int u = 0;
                u++;

          /*      ProjectManager.Instance.OpenProject(openFileDialog.FileName);
                GameProperties gameProperties = ProjectManager.Instance.CurrentProject.GetGamePropertiesInfo();
                gamePropertyGrid.SelectedObject = gameProperties;

                ProjectManager.Instance.CurrentProject.GetResourcesXMLPath();

                foreach (String resource in ProjectManager.Instance.GetResourceNames())
                {
                    TreeNode treeNode = resourceTreeView.Nodes.Add(resource);
                }
           */
            }
        }

        private void closeToolStripMenuItem1_Click(object sender, EventArgs e)
        {
        //    ProjectManager.Instance.CloseProject();
      //      gamePropertyGrid.SelectedObject = null;
         //   resourceTreeView.Nodes.Clear();
        }

        private void newFolderToolStripMenuItem_Click(object sender, EventArgs e)
        {
            
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
    
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
      //      ProjectManager.Instance.SaveProject();
        }

        private void treeView1_NodeMouseClick(object sender, TreeNodeMouseClickEventArgs e)
        {
     /*       if (resourceTreeView.SelectedNode!=null)
            {
                String NodeName = resourceTreeView.SelectedNode.Text;
                ResourceImageDefault res = ProjectManager.Instance.resourceManager.GetResource(NodeName);

                resourcePropertyGrid.SelectedObject = res;
            }*/
        }

        private void compileToolStripMenuItem_Click(object sender, EventArgs e)
        {
     /*       using (CompileProjectForm compileProjectForm = new CompileProjectForm())
            {
                if (compileProjectForm.ShowDialog() == DialogResult.OK)
                {
                }
            }*/
        }

        private void fileToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }
    }
}