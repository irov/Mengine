using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Windows.Forms;
using System.Xml;

namespace ProjectManager
{
    public sealed class ProjectManager
    {
        public static String TemplateFolderName = "templates";

        private Dictionary<String, String> AvaibleTemplates;

        private ProjectManager()
        {
            AvaibleTemplates = new Dictionary<String, String>();
        }

        public static ProjectManager Instance
        {
            get
            {
                return Nested.instance;
            }
        }

        class Nested
        {
            static Nested()
            {}
            
            internal static readonly ProjectManager instance = new ProjectManager();
        }

        public String GetProjectManagerDir()
        {
            return ProjectManagerDirectory;
        }

        public String GetCurrentProjectDir()
        {
            return CurrentProject.GetProjectDirectory();
        }

        public String[] GetAvaibleTemplates()
        {
            String templatePath = System.IO.Path.Combine(ProjectManagerDirectory, TemplateFolderName);

            DirectoryInfo currDirInfo = new DirectoryInfo(templatePath);

            DirectoryInfo[] directories = currDirInfo.GetDirectories();

            foreach (DirectoryInfo directory in directories)
            {
                if (directory.Name == ".svn")
                {
                    continue;
                }

                if (!AvaibleTemplates.ContainsKey(directory.Name))
                {
                    AvaibleTemplates.Add(directory.Name, directory.FullName);
                }
            }

            String[] array = new String[AvaibleTemplates.Count];
            AvaibleTemplates.Keys.CopyTo(array, 0);
            return array;
        }

        public String[] GetResourceNames()
        {
            String[] array = new String[resourceManager.GetCount()];
            resourceManager.GetKeys().CopyTo(array, 0);
            return array;
        }

        public void SaveProject()
        {
            if (CurrentProject == null)
            {
                return;
            }

            CurrentProject.Save();
        }

        public void CreateNewProject(String name, String path, String template)
        {
            resourceManager = new ResourceManager();

            String templateFullPath = ProjectManager.Instance.GetFullPathTemplate(template);
            String newProjectFullPath = System.IO.Path.Combine(path, name);

            FileSystem.CopyDirectory(templateFullPath, newProjectFullPath);

            String HACK = System.IO.Path.Combine(newProjectFullPath, "Bin\\Application.xml");
            OpenProject(HACK);
        }

        public void RunCurrentProject(String args)
        {
            if (CurrentProject == null)
            {
                MessageBox.Show("there's no project opened!");
                return;
            }

            SaveProject();

            String ProjectFullPath
                = CurrentProject.GetProjectDirectory();

            System.IO.Directory.SetCurrentDirectory(ProjectFullPath + "\\Bin\\");

            System.Diagnostics.ProcessStartInfo psi =
                new System.Diagnostics.ProcessStartInfo(ProjectFullPath+"\\Bin\\Game.exe");

            psi.RedirectStandardOutput = false;
            psi.Arguments = args;
        
            System.Diagnostics.Process listFiles;
            listFiles = System.Diagnostics.Process.Start(psi);

            listFiles.WaitForExit();

            if (listFiles.HasExited)
            {
                System.IO.Directory.SetCurrentDirectory(ProjectManagerDirectory);
            }           
        }

        public void OpenProject(String appname)
        {
            resourceManager = new ResourceManager();

            Project newProject = new Project();

            newProject.Load(appname);

            String ResourcePath = newProject.GetResourcePath();

            resourceManager.loadResource(ResourcePath);
            
            CurrentProject = newProject;
        }

        public void CloseProject()
        {
            CurrentProject = null;
        }

        public void InitProjectManagerDir()
        {
            ProjectManagerDirectory = System.IO.Directory.GetCurrentDirectory();
        }

        private String GetFullPathTemplate(String name)
        {
            String FullPath;

            if (AvaibleTemplates.TryGetValue(name, out FullPath))
            {
                return FullPath;
            }

            return "";
        }

        public Project CurrentProject = null;
        private String ProjectManagerDirectory;
        public ResourceManager resourceManager = null;
    }
}