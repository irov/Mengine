using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Xml;

public delegate void onBuildJobCallback();

namespace MengeProjectBuilder
{
    public partial class Form1 : Form
    {
        private LogWindow m_logWindow;

        public Form1()
        {
            m_utilsPath = System.IO.Directory.GetCurrentDirectory();
            m_svnBin = m_utilsPath + System.IO.Path.DirectorySeparatorChar + "svn" + System.IO.Path.DirectorySeparatorChar + "bin";
            InitializeComponent();
            foreach (string item in Properties.Settings.Default.DefaultSourceList)
            {
                cmb_src.Items.Add(item);
            }
            if (cmb_src.Text.Length != 0
                && txtBox_dst.Text.Length != 0)
            {
                m_buttonBuild.Enabled = true;
            }
            else
            {
                m_buttonBuild.Enabled = false;
            }
            //Utils.copyDirectory("fad", "fadf");

            m_logWindow = new LogWindow();
        }

        private void btn_src_Click(object sender, EventArgs e)
        {
            brsDlg_src.SelectedPath = cmb_src.Text;
            DialogResult res = brsDlg_src.ShowDialog();
            if (res == DialogResult.OK)
            {
                cmb_src.Text = brsDlg_src.SelectedPath;
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            brsDlg_dst.SelectedPath = txtBox_dst.Text;
            DialogResult res = brsDlg_dst.ShowDialog();
            if (res == DialogResult.OK)
            {
                txtBox_dst.Text = brsDlg_dst.SelectedPath;
            }
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {
            if (cmb_src.Text.Length != 0
                && txtBox_dst.Text.Length != 0)
            {
                m_buttonBuild.Enabled = true;
            }
            else
            {
                m_buttonBuild.Enabled = false;
            }
        }

        private void textBox2_TextChanged(object sender, EventArgs e)
        {
            if (cmb_src.Text.Length != 0
                && txtBox_dst.Text.Length != 0)
            {
                m_buttonBuild.Enabled = true;
            }
            else
            {
                m_buttonBuild.Enabled = false;
            }

        }

        private void m_buttonBuild_Click(object sender, EventArgs e)
        {
            if (System.IO.File.Exists(m_svnBin + System.IO.Path.DirectorySeparatorChar + "svn.exe") == false)
            {
                MessageBox.Show("SVN Tool not found", "Critical Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            if (cmb_src.Items.Contains(cmb_src.Text) == false)
            {
                cmb_src.Items.Add(cmb_src.Text);
                Properties.Settings.Default.DefaultSourceList.Add(cmb_src.Text);
            }

            m_buttonBuild.Visible = false;
            btn_Cancel.Visible = true;
            m_logWindow.Show();
            m_logWindow.Clear();

            string dstDir = txtBox_dst.Text;

            BuildThread buildThread = new BuildThread(m_logWindow, m_utilsPath, m_svnBin, cmb_src.Text, dstDir,
                chk_trimAlpha.Checked, num_maxAlphaValue.Value, chk_alphaEdgeCorrection.Checked,
                chk_atlas.Checked, num_atlasMaxSize.Value, num_atlasImageMaxSize.Value,
                m_trimAtlasesCheck.Checked,
                chk_convert.Checked, num_jpegQual.Value,
                chk_makePaks.Checked, m_companyNameEdit.Text,
                onBuildJobEnd);
            m_thread = new System.Threading.Thread(new System.Threading.ThreadStart(buildThread.buildJob));
            try
            {
                m_thread.Start();
            }
            catch (System.Exception ex)
            {
                MessageBox.Show(ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

            //build();
            //m_buttonBuild.Enabled = true;
        }

        private void onBuildJobEnd()
        {
            SetControlPropertyValue(m_buttonBuild, "Visible", true);
            SetControlPropertyValue(btn_Cancel, "Visible", false);
            System.IO.Directory.SetCurrentDirectory(m_utilsPath);
            //this.Enabled = true;
        }

        private void SetControlPropertyValue(Control oControl, string propName, object propValue)
        {
            if (oControl.InvokeRequired)
            {
                SetControlValueCallback d = new SetControlValueCallback(SetControlPropertyValue);
                oControl.Invoke(d, new object[] { oControl, propName, propValue });
            }
            else
            {
                Type t = oControl.GetType();
                System.Reflection.PropertyInfo[] props = t.GetProperties();
                foreach (System.Reflection.PropertyInfo p in props)
                {
                    if (p.Name.ToUpper() == propName.ToUpper())
                    {
                        p.SetValue(oControl, propValue, null);
                    }
                }
            }
        }

        private void chk_atlas_CheckedChanged(object sender, EventArgs e)
        {
            bool check = chk_atlas.Checked;
            label3.Enabled = check;
            num_atlasMaxSize.Enabled = check;
            label4.Enabled = check;
            num_atlasImageMaxSize.Enabled = check;
            m_trimAtlasesCheck.Enabled = check;
        }

        private void Form1_Load(object sender, EventArgs e)
        {
        }

        private void btn_Cancel_Click(object sender, EventArgs e)
        {
            /*if (m_thread != null)
            {
                m_thread.Abort();
            }*/
        }

        private void chk_trimAlpha_CheckedChanged(object sender, EventArgs e)
        {
            bool check = chk_trimAlpha.Checked;
            label5.Enabled = check;
            num_maxAlphaValue.Enabled = check;
            chk_alphaEdgeCorrection.Enabled = check;
        }

        private void chk_convert_CheckedChanged(object sender, EventArgs e)
        {
            bool check = chk_convert.Checked;
            label6.Enabled = check;
            num_jpegQual.Enabled = check;
        }

    }
    public class API
    {
        [DllImport("msvcrt.dll", SetLastError = false, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int sprintf(StringBuilder buff, string format, __arglist);
    }

    public class BuildThread
    {
        private string m_utilsPath;
        private string m_svnBin;
        private string m_srcDir;
        private string m_dstDir;
        private bool m_trimAlpha;
        private decimal m_maxAlphaValue;
        private bool m_alphaEdgeCorrection;
        private bool m_makeAtlases;
        private decimal m_atlasMaxSize;
        private decimal m_atlasImageMaxSize;
        private bool m_trimAtlases;
        private bool m_mneConvert;
        private decimal m_jpegQuality;
        private bool m_makePak;
        private onBuildJobCallback m_onEndCallback;
        private LogWindow m_logWindow;
        private string m_companyName;

        private delegate void logMessageDelegate(string _message, Color _color);

        public BuildThread(LogWindow _logWindow, string _utilsPath, string _svnBin, string _srcDir, string _dstDir,
            bool _trimAlpha, decimal _maxAlphaValue, bool _alphaEdgeCorrection,
            bool _makeAtlases, decimal _atlasMaxSize, decimal _atlasImageMaxSize,
            bool _trimAltases,
            bool _mneConvert, decimal _jpegQuality,
            bool _makePak, string _companyName,
            onBuildJobCallback _callback)
        {
            m_logWindow = _logWindow;
            m_utilsPath = _utilsPath;
            m_svnBin = _svnBin;
            m_srcDir = _srcDir;
            m_dstDir = _dstDir;
            m_trimAlpha = _trimAlpha;
            m_maxAlphaValue = _maxAlphaValue;
            m_alphaEdgeCorrection = _alphaEdgeCorrection;
            m_makeAtlases = _makeAtlases;
            m_atlasMaxSize = _atlasMaxSize;
            m_atlasImageMaxSize = _atlasImageMaxSize;
            m_trimAtlases = _trimAltases;
            m_mneConvert = _mneConvert;
            m_jpegQuality = _jpegQuality;
            m_makePak = _makePak;
            m_companyName = _companyName;
            m_onEndCallback = _callback;
        }

        public void logMessage(string _message, Color _color)
        {
            m_logWindow.Invoke(new logMessageDelegate(m_logWindow.logMessage), new object[] { _message, _color });
        }

        public class ResourceImages
        {
            public string resourceFileName;
            public XmlDocument resourceXml;
            public Dictionary<string, System.Collections.ArrayList> imageNodeDict;
        }

        public Dictionary<string, System.Collections.ArrayList>
            getImageNodeDictionary(XmlDocument _resourceXml, string[] _resourceTypes)
        {
            XmlNodeList resourceList = _resourceXml.GetElementsByTagName("Resource");
            System.Collections.Generic.Dictionary<string, System.Collections.ArrayList> pathDict =
                new System.Collections.Generic.Dictionary<string, System.Collections.ArrayList>();
            foreach (XmlNode resourceNode in resourceList)
            {
                bool checkResourceType = false;
                string myResType = resourceNode.SelectSingleNode("@Type").Value;
                foreach (string resType in _resourceTypes)
                {
                    if (myResType == resType)
                    {
                        checkResourceType = true;
                        break;
                    }
                }
                if (checkResourceType == false)
                {
                    continue;
                }
                XmlNodeList childs = resourceNode.ChildNodes;
                foreach (XmlNode child in childs)
                {
                    if (child.Name != "File"
                        && child.Name != "Image")
                    {
                        continue;
                    }
                    string path = "";
                    if (child.Attributes.GetNamedItem("Path") != null)
                    {
                        path = child.Attributes.GetNamedItem("Path").Value;
                        if (path == "CreateTarget" || path == "CreateImage")
                        {
                            continue;
                        }
                        else if (path.IndexOf(' ') != -1)
                        {
                            continue;
                        }
                    }

                    if (pathDict.ContainsKey(path) == false)
                    {
                        pathDict.Add(path, new System.Collections.ArrayList());
                    }
                    pathDict[path].Add(resourceNode);
                }
            }
            return pathDict;
        }

        public void buildJob()
        {
            logMessage("Build started...\n", Color.Green);

            if (System.IO.Directory.Exists(m_dstDir) == false)
            {
                DialogResult res = MessageBox.Show("Destination directory does not exist.\n Procceed creating new folder?"
                    , "Warning"
                    , MessageBoxButtons.YesNo
                    , MessageBoxIcon.Warning);
                if (res == DialogResult.No)
                {
                    m_onEndCallback();
                    return;
                }

                System.IO.Directory.CreateDirectory(m_dstDir);
            }

            System.IO.DirectoryInfo dest_dir_info = new System.IO.DirectoryInfo(m_dstDir);
            if (dest_dir_info.GetDirectories().Length > 0
                || dest_dir_info.GetFiles().Length > 0)
            {
                DialogResult res = MessageBox.Show("Destination directory does not empty.\n Procceed erasing all it's content?"
                   , "Warning"
                   , MessageBoxButtons.YesNo
                   , MessageBoxIcon.Warning);
                if (res == DialogResult.No)
                {
                    m_onEndCallback();
                    return;
                }

                try
                {
                    logMessage("Deleting...\n", Color.Black);
                    System.IO.Directory.Delete(m_dstDir, true);
                }
                catch (System.Exception exp)
                {
                    MessageBox.Show(exp.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    m_onEndCallback();
                    return;
                }
                System.IO.Directory.CreateDirectory(m_dstDir);
            }

            logMessage("Exporting...\n", Color.Black);
            if (svn_export(m_srcDir, m_dstDir) != 0)
            {
                m_onEndCallback();
                return;
            }

            // parse Application.xml
            // System.IO.DirectoryInfo DestDirInfo = new System.IO.DirectoryInfo( textBox2.Text );
            //System.IO.DirectoryInfo dest_dir_info = new System.IO.DirectoryInfo(m_dstDir);
            System.IO.FileInfo AppXmlInfo = Utils.searchFile(dest_dir_info, "application.xml");
            if (AppXmlInfo == null)
            {
                logMessage("Error: \"application.xml\" not found\n", Color.Red);
                m_onEndCallback();
                return;
            }

            System.Collections.ArrayList gamePaks = new System.Collections.ArrayList();

            XmlDocument appXmlDoc = new XmlDocument();
            appXmlDoc.Load(AppXmlInfo.FullName);
            string baseDir = Utils.GetNodeAttribute(appXmlDoc, "BaseDir", "Value");
            baseDir.Replace('/', '\\');
            string gamePack = Utils.GetNodeAttribute(appXmlDoc, "GamePack", "Path");
            gamePack.Replace('/', '\\');
            string gameFile = Utils.GetNodeAttribute(appXmlDoc, "GamePack", "Description");
            gameFile.Replace('/', '\\');

            gamePaks.Add(gamePack);

            if (m_makePak == true)
            {
                XmlNodeList nodeList = appXmlDoc.GetElementsByTagName("GamePack");
                nodeList[0].Attributes.GetNamedItem("Path").Value = gamePack.Replace('\\', '/') + ".pak";
                nodeList = appXmlDoc.GetElementsByTagName("BaseDir");
                nodeList[0].Attributes.GetNamedItem("Value").Value = "";
                appXmlDoc.Save(AppXmlInfo.FullName);
            }

            // set gamePack dir
            System.IO.Directory.SetCurrentDirectory(AppXmlInfo.DirectoryName);
            System.IO.Directory.SetCurrentDirectory(baseDir + gamePack);

            // parse game.ini
            System.Collections.ArrayList iniFileLines = new System.Collections.ArrayList();
            System.Collections.Generic.Dictionary<string, string> resourceFiles = new System.Collections.Generic.Dictionary<string, string>();
            string iconFile = "";
            string projectName = "";
            XmlDocument GameXmlDoc = new XmlDocument();
            GameXmlDoc.Load(gameFile);
            iconFile = Utils.GetNodeAttribute(GameXmlDoc, "Icon", "Path");
            iconFile = iconFile.Replace('/', '\\');
            XmlNodeList iconNodeList = GameXmlDoc.GetElementsByTagName("Icon");
            if (iconNodeList.Count > 0)
            {
                iconNodeList[0].ParentNode.RemoveChild(iconNodeList[0]);
            }
            projectName = Utils.GetNodeAttribute(GameXmlDoc, "Title", "Value");
            XmlNodeList resourceNodeList = GameXmlDoc.GetElementsByTagName("ResourcePack");
            foreach (XmlNode resNode in resourceNodeList)
            {
                string name = resNode.Attributes.GetNamedItem("Name").Value;
                string pack = resNode.Attributes.GetNamedItem("Path").Value;
                if (gamePaks.BinarySearch(pack) < 0)
                {
                    gamePaks.Add(pack);
                }
                string resourceFile = pack + "/" + resNode.Attributes.GetNamedItem("Description").Value;
                resourceFile = resourceFile.Replace('/', '\\');
                resourceFiles.Add(name, resourceFile);
                if (m_makePak == true)
                {
                    resNode.Attributes.GetNamedItem("Path").Value += ".pak";
                }
            }
            resourceNodeList = GameXmlDoc.GetElementsByTagName("LanguagePack");
            foreach (XmlNode resNode in resourceNodeList)
            {
                string name = resNode.Attributes.GetNamedItem("Name").Value;
                string pack = resNode.Attributes.GetNamedItem("Path").Value;
                gamePaks.Add(pack);
                string resourceFile = pack + "/" + resNode.Attributes.GetNamedItem("Description").Value;
                resourceFile = resourceFile.Replace('/', '\\');
                resourceFiles.Add(name, resourceFile);
                if (m_makePak == true)
                {
                    resNode.Attributes.GetNamedItem("Path").Value += ".pak";
                }
            }
            GameXmlDoc.Save(gameFile);

            // parse resources
            foreach (KeyValuePair<string, string> resPair in resourceFiles)
            {
                string resourceFile = resPair.Value;

                System.IO.Directory.SetCurrentDirectory(AppXmlInfo.DirectoryName);
                System.IO.Directory.SetCurrentDirectory(baseDir);

                string fileName = System.IO.Path.GetFullPath(resourceFile);
                string dirName = System.IO.Path.GetDirectoryName(fileName);

                System.IO.Directory.SetCurrentDirectory(dirName);
                XmlDocument resourceXml = new XmlDocument();
                resourceXml.Load(fileName);
                // try to find texts first and search for project name
                string textPath = Utils.GetNodeAttribute(resourceXml, "Text", "Path");
                if (textPath != null
                    && textPath != "")
                {
                    XmlDocument textXml = new XmlDocument();
                    textXml.Load(textPath.Replace('/', '\\'));
                    XmlNodeList textNodeList = textXml.GetElementsByTagName("Text");
                    foreach (XmlNode textNode in textNodeList)
                    {
                        XmlNode keyNode = textNode.Attributes.GetNamedItem("Key");
                        if (keyNode != null
                            && keyNode.Value == projectName)
                        {
                            projectName = textNode.Attributes.GetNamedItem("Value").Value;
                        }
                    }
                }

                if (resourceXml.GetElementsByTagName("Resource").Count == 0)
                {
                    continue;
                }
                string resDir = Utils.GetNodeAttribute(resourceXml, "Resource", "Path");
                XmlNode resNode = resourceXml.GetElementsByTagName("Resource").Item(0);
                // get all image resource in this pack
                System.Collections.ArrayList resourceFilesXml = new System.Collections.ArrayList();
                for (int i = 0; i < resNode.ChildNodes.Count; ++i)
                {
                    XmlDocument resourceXmlDoc = new XmlDocument();
                    ResourceImages resImgs = new ResourceImages();
                    resImgs.resourceFileName = resDir + System.IO.Path.DirectorySeparatorChar + resNode.ChildNodes.Item(i).Name + ".resource";
                    resourceXmlDoc.Load(resImgs.resourceFileName);
                    resImgs.resourceXml = resourceXmlDoc;
                    //resImgs.imageNodeDict = getImageNodeDictionary(resourceXmlDoc, new string[] { "ResourceImageDefault" });
                    resourceFilesXml.Add(resImgs);
                }
                //foreach (ResourceImages resImage in resourceFilesXml)
                for (int i = 0; i < resourceFilesXml.Count; ++i)
                {
                    ResourceImages resImage = resourceFilesXml[i] as ResourceImages;

                    unwideFileNames(resImage.resourceXml);
                    makeLowerRegister(resImage.resourceXml);
                    checkRegister(resImage.resourceXml);
                    // particles
                    resImage.imageNodeDict = getImageNodeDictionary(resImage.resourceXml, new string[] { "ResourceEmitterContainer" });

                    extract_particle_textures(resImage, resPair.Key);

                    if (m_trimAlpha == true)
                    {
                        resImage.imageNodeDict = getImageNodeDictionary(resImage.resourceXml, new string[] { "ResourceImageDefault" });
                        trim_alpha(resImage, m_maxAlphaValue, m_alphaEdgeCorrection);
                    }
                    if (m_makeAtlases == true)
                    {
                        resImage.imageNodeDict = getImageNodeDictionary(resImage.resourceXml, new string[] { "ResourceImageDefault" });
                        make_atlas(resImage, m_atlasMaxSize, m_atlasImageMaxSize, m_trimAtlases);
                    }
                    if (m_mneConvert == true)
                    {
                        resImage.imageNodeDict = getImageNodeDictionary(resImage.resourceXml, new string[] { "ResourceImageDefault", "ResourceFont", "ResourceWindow" });
                        mne_convert(resImage, m_jpegQuality);
                    }

                    // save resourceFile
                    resImage.resourceXml.Save(resImage.resourceFileName);
                }
            }

            // compile .py -> .pyo
            System.IO.Directory.SetCurrentDirectory(m_dstDir);
            string[] pyFiles = System.IO.Directory.GetFiles(m_dstDir, "*.py", System.IO.SearchOption.AllDirectories);
            string pyFilesString = "";
            foreach (string pyFile in pyFiles)
            {
                pyFilesString += " " + pyFile;
            }
            string pythonPath = m_utilsPath + System.IO.Path.DirectorySeparatorChar + "python";
            System.Diagnostics.Process py_compile_proc = new System.Diagnostics.Process();
            py_compile_proc.StartInfo.FileName = pythonPath
                + System.IO.Path.DirectorySeparatorChar + "python.exe";
            py_compile_proc.StartInfo.Arguments = "-O " + pythonPath + System.IO.Path.DirectorySeparatorChar + "py_compile.py" + pyFilesString;
            py_compile_proc.StartInfo.UseShellExecute = false;
            py_compile_proc.StartInfo.RedirectStandardOutput = true;
            py_compile_proc.StartInfo.RedirectStandardError = true;
            py_compile_proc.StartInfo.CreateNoWindow = true;
            py_compile_proc.EnableRaisingEvents = true;
            py_compile_proc.ErrorDataReceived += new System.Diagnostics.DataReceivedEventHandler(proccess_ErrorDataReceived);
            py_compile_proc.OutputDataReceived += new System.Diagnostics.DataReceivedEventHandler(proccess_OutputDataReceived);
            py_compile_proc.Start();
            py_compile_proc.BeginOutputReadLine();
            py_compile_proc.BeginErrorReadLine();
            py_compile_proc.WaitForExit();

            foreach (string pyFile in pyFiles)
            {
                System.IO.File.Delete(pyFile);
            }

            string binDir = AppXmlInfo.Directory.Name;
            if (binDir.ToUpper() == "BIN"
                && (iconFile != "" || projectName != ""))
            {
                logMessage("Patching resources...\n", Color.Black);
                patchWin32(binDir + System.IO.Path.DirectorySeparatorChar + "Game.exe"
                    , iconFile, projectName);
            }

            if (m_makePak == true)
            {
                logMessage("Packing...\n", Color.Black);
                foreach (string pack in gamePaks)
                {
                    System.IO.Directory.SetCurrentDirectory(AppXmlInfo.DirectoryName);
                    System.IO.Directory.SetCurrentDirectory(baseDir + pack);

                    System.Diagnostics.Process zip_proc = new System.Diagnostics.Process();
                    zip_proc.StartInfo.Arguments = "a -tzip -r -bd ..\\" + pack + ".pak *.* -mx0";
                    zip_proc.StartInfo.FileName = m_utilsPath + System.IO.Path.DirectorySeparatorChar + "7z";
                    zip_proc.StartInfo.UseShellExecute = false;
                    zip_proc.StartInfo.RedirectStandardOutput = true;
                    zip_proc.StartInfo.RedirectStandardError = true;
                    zip_proc.StartInfo.CreateNoWindow = true;
                    zip_proc.EnableRaisingEvents = true;
                    zip_proc.ErrorDataReceived += new System.Diagnostics.DataReceivedEventHandler(proccess_ErrorDataReceived);
                    zip_proc.OutputDataReceived += new System.Diagnostics.DataReceivedEventHandler(proccess_OutputDataReceived);
                    zip_proc.Start();
                    zip_proc.BeginOutputReadLine();
                    zip_proc.BeginErrorReadLine();
                    zip_proc.WaitForExit();

                    System.IO.Directory.SetCurrentDirectory(AppXmlInfo.DirectoryName);
                    System.IO.Directory.SetCurrentDirectory(baseDir);
                    System.IO.Directory.Delete(pack, true);
                }
                System.IO.Directory.SetCurrentDirectory(AppXmlInfo.DirectoryName);
                System.IO.Directory.SetCurrentDirectory(baseDir);
                Utils.copyDirectory(AppXmlInfo.DirectoryName, ".");
                System.IO.Directory.Delete(AppXmlInfo.DirectoryName, true);

            }

            logMessage("Builded successfully!\n", Color.Green);
            m_onEndCallback();
        }

        private int svn_export(string _srcDir, string _dstDir)
        {
            string svn_arguments = "export --force " + _srcDir + " " + _dstDir;
            System.Diagnostics.Process svn_export_proc = new System.Diagnostics.Process();
            svn_export_proc.StartInfo.Arguments = svn_arguments;
            svn_export_proc.StartInfo.FileName = m_svnBin + System.IO.Path.DirectorySeparatorChar + "svn";
            svn_export_proc.StartInfo.UseShellExecute = false;
            svn_export_proc.StartInfo.RedirectStandardOutput = true;
            svn_export_proc.StartInfo.RedirectStandardError = true;
            svn_export_proc.StartInfo.CreateNoWindow = true;
            svn_export_proc.EnableRaisingEvents = true;
            svn_export_proc.ErrorDataReceived += new System.Diagnostics.DataReceivedEventHandler(proccess_ErrorDataReceived);
            svn_export_proc.OutputDataReceived += new System.Diagnostics.DataReceivedEventHandler(proccess_OutputDataReceived);
            svn_export_proc.Start();
            svn_export_proc.BeginOutputReadLine();
            svn_export_proc.BeginErrorReadLine();
            svn_export_proc.WaitForExit();
            return svn_export_proc.ExitCode;
        }

        void proccess_ErrorDataReceived(object sender, System.Diagnostics.DataReceivedEventArgs e)
        {
            if (e.Data != null)
                logMessage(e.Data + "\n", Color.Red);
        }

        void proccess_OutputDataReceived(object sender, System.Diagnostics.DataReceivedEventArgs e)
        {
            if (e.Data != null)
                logMessage(e.Data + "\n", Color.Black);
        }

        private bool make_atlas(ResourceImages _resImages, decimal _atlasMaxSize, decimal _atlasImageMaxSize, bool _trim)
        {
            if (System.IO.File.Exists(m_utilsPath + System.IO.Path.DirectorySeparatorChar + "AtlasTool.exe") == false)
            {
                logMessage("AtalsTool.exe not found", Color.Red);
                return false;
            }
            if (_resImages.imageNodeDict.Keys.Count < 2)
            {
                return true;
            }
            System.Diagnostics.Process atlas_tool_proc = new System.Diagnostics.Process();
            atlas_tool_proc.StartInfo.FileName = m_utilsPath + System.IO.Path.DirectorySeparatorChar + "AtlasTool";
            atlas_tool_proc.StartInfo.UseShellExecute = false;
            atlas_tool_proc.StartInfo.RedirectStandardOutput = true;
            atlas_tool_proc.StartInfo.RedirectStandardError = true;
            atlas_tool_proc.StartInfo.CreateNoWindow = true;
            atlas_tool_proc.EnableRaisingEvents = true;
            atlas_tool_proc.ErrorDataReceived += new System.Diagnostics.DataReceivedEventHandler(proccess_ErrorDataReceived);
            atlas_tool_proc.OutputDataReceived += new System.Diagnostics.DataReceivedEventHandler(proccess_OutputDataReceived);


            System.IO.StreamWriter inputTxt = new System.IO.StreamWriter("input.txt");
            foreach (string filePath in _resImages.imageNodeDict.Keys)
            {
                bool writeToAtlas = true;
                foreach (XmlNode resourceNode in _resImages.imageNodeDict[filePath])
                {
                    XmlNode xmlNoAtlas = resourceNode.SelectSingleNode("*[@Path=\"" + filePath + "\"]/@NoAtlas");
                    if (xmlNoAtlas != null
                        && (xmlNoAtlas.Value == "1" || xmlNoAtlas.Value.ToLower() == "true"))
                    {
                        writeToAtlas = false;
                        break;
                    }
                    XmlNode xmlAtlasWrapX = resourceNode.SelectSingleNode("*[@Path=\"" + filePath + "\"]/@WrapX");
                    if (xmlAtlasWrapX != null
                        && (xmlAtlasWrapX.Value == "1" || xmlAtlasWrapX.Value.ToLower() == "true"))
                    {
                        writeToAtlas = false;
                        break;
                    }
                    XmlNode xmlAtlasWrapY = resourceNode.SelectSingleNode("*[@Path=\"" + filePath + "\"]/@WrapY");
                    if (xmlAtlasWrapY != null
                        && (xmlAtlasWrapY.Value == "1" || xmlAtlasWrapY.Value.ToLower() == "true"))
                    {
                        writeToAtlas = false;
                        break;
                    }
                }
                if (writeToAtlas == true)
                {
                    inputTxt.WriteLine(filePath);
                }
            }
            inputTxt.Close();

            atlas_tool_proc.StartInfo.Arguments = "";
            if (_trim == true)
            {
                atlas_tool_proc.StartInfo.Arguments = "/t ";
            }
            atlas_tool_proc.StartInfo.Arguments += "/a:" + _atlasMaxSize.ToString()
                + " /i:" + _atlasImageMaxSize.ToString()
                + " " + System.IO.Path.GetFileNameWithoutExtension(_resImages.resourceFileName)
                + " input.txt output.txt";
            atlas_tool_proc.Start();
            atlas_tool_proc.BeginOutputReadLine();
            atlas_tool_proc.BeginErrorReadLine();
            atlas_tool_proc.WaitForExit();
            if (atlas_tool_proc.ExitCode != 0)
            {
                return false;
            }

            System.IO.StreamReader outputTxt = new System.IO.StreamReader("output.txt");
            string line = outputTxt.ReadLine();
            while (line != null)
            {
                string[] lineSplit = line.Split(' ');
                System.IO.File.Delete(lineSplit[0]);
                System.Collections.ArrayList resNodeList = _resImages.imageNodeDict[lineSplit[0]];
                _resImages.imageNodeDict.Remove(lineSplit[0]);
                if (_resImages.imageNodeDict.ContainsKey(lineSplit[1]) == false)
                {
                    _resImages.imageNodeDict.Add(lineSplit[1], resNodeList);
                }
                else
                {
                    _resImages.imageNodeDict[lineSplit[1]].AddRange(resNodeList);
                }
                foreach (XmlNode resNode in resNodeList)
                {
                    XmlNodeList childs = resNode.ChildNodes;
                    foreach (XmlNode child in childs)
                    {
                        if (child.Name != "File")
                        {
                            continue;
                        }
                        XmlNode pathNode = child.Attributes.GetNamedItem("Path");
                        if (pathNode != null
                            && pathNode.Value == lineSplit[0])
                        {
                            pathNode.Value = lineSplit[1];
                            XmlAttribute attr = _resImages.resourceXml.CreateAttribute("UV");
                            attr.Value = lineSplit[2] + ";" + lineSplit[3] + ";" + lineSplit[4] + ";" + lineSplit[5];
                            child.Attributes.Append(attr);
                        }

                    }
                }
                line = outputTxt.ReadLine();
            }
            outputTxt.Close();

            // remove input.txt and output.txt
            System.IO.File.Delete("input.txt");
            System.IO.File.Delete("output.txt");

            return true;
        }

        private bool extract_particle_textures(ResourceImages _resImages, string _pack)
        {
            if (System.IO.File.Exists(m_utilsPath + System.IO.Path.DirectorySeparatorChar + "MagicExtractor.exe") == false)
            {
                logMessage("MagicExtractor.exe not found", Color.Red);
                return false;
            }

            System.Diagnostics.Process ex_tool_proc = new System.Diagnostics.Process();
            ex_tool_proc.StartInfo.FileName = m_utilsPath + System.IO.Path.DirectorySeparatorChar + "MagicExtractor";
            ex_tool_proc.StartInfo.UseShellExecute = false;
            ex_tool_proc.StartInfo.RedirectStandardOutput = true;
            ex_tool_proc.StartInfo.RedirectStandardError = true;
            ex_tool_proc.StartInfo.CreateNoWindow = true;
            ex_tool_proc.EnableRaisingEvents = true;
            ex_tool_proc.ErrorDataReceived += new System.Diagnostics.DataReceivedEventHandler(proccess_ErrorDataReceived);
            ex_tool_proc.OutputDataReceived += new System.Diagnostics.DataReceivedEventHandler(proccess_OutputDataReceived);

            foreach (string filename in _resImages.imageNodeDict.Keys)
            {
                string partFolder = System.IO.Path.GetDirectoryName(filename);
                ex_tool_proc.StartInfo.Arguments = filename + " part_textures.txt";
                ex_tool_proc.Start();
                ex_tool_proc.BeginOutputReadLine();
                ex_tool_proc.BeginErrorReadLine();
                ex_tool_proc.WaitForExit();
                ex_tool_proc.CancelOutputRead();
                ex_tool_proc.CancelErrorRead();

                System.IO.StreamReader partTextures = new System.IO.StreamReader("part_textures.txt");
                string line = partTextures.ReadLine();
                while (line != null)
                {
                    XmlElement xmlResource = _resImages.resourceXml.CreateElement("Resource");
                    XmlAttribute xmlNameAttrib = _resImages.resourceXml.CreateAttribute("Name");
                    xmlNameAttrib.Value = _pack + "/" + partFolder + "/" + line;
                    XmlAttribute xmlTypeAttrib = _resImages.resourceXml.CreateAttribute("Type");
                    xmlTypeAttrib.Value = "ResourceImageDefault";
                    xmlResource.Attributes.Append(xmlNameAttrib);
                    xmlResource.Attributes.Append(xmlTypeAttrib);
                    XmlElement xmlFileElement = _resImages.resourceXml.CreateElement("File");
                    XmlAttribute xmlFilePath = _resImages.resourceXml.CreateAttribute("Path");
                    xmlFilePath.Value = partFolder + "/" + line;
                    xmlFileElement.Attributes.Append(xmlFilePath);
                    xmlResource.AppendChild(xmlFileElement);
                    _resImages.resourceXml.FirstChild.AppendChild(xmlResource);
                    line = partTextures.ReadLine();
                }
                partTextures.Close();
                System.IO.File.Delete("part_textures.txt");

            }
            return true;
        }

        private bool trim_alpha(ResourceImages _resImages, decimal _maxAlphaValue, bool _alphaEdgeCorrection)
        {
            if (System.IO.File.Exists(m_utilsPath + System.IO.Path.DirectorySeparatorChar + "TrimAlphaTool.exe") == false)
            {
                logMessage("TrimAlphaTool.exe not found", Color.Red);
                return false;
            }
            System.Diagnostics.Process trim_alpha_proc = new System.Diagnostics.Process();
            trim_alpha_proc.StartInfo.FileName = m_utilsPath + System.IO.Path.DirectorySeparatorChar + "TrimAlphaTool";
            trim_alpha_proc.StartInfo.UseShellExecute = false;
            trim_alpha_proc.StartInfo.RedirectStandardOutput = true;
            trim_alpha_proc.StartInfo.RedirectStandardError = true;
            trim_alpha_proc.StartInfo.CreateNoWindow = true;
            trim_alpha_proc.EnableRaisingEvents = true;
            trim_alpha_proc.ErrorDataReceived += new System.Diagnostics.DataReceivedEventHandler(proccess_ErrorDataReceived);
            trim_alpha_proc.OutputDataReceived += new System.Diagnostics.DataReceivedEventHandler(proccess_OutputDataReceived);


            System.IO.StreamWriter inputTxt = new System.IO.StreamWriter("input.txt");
            foreach (string filePath in _resImages.imageNodeDict.Keys)
            {
                inputTxt.WriteLine(filePath);
            }
            inputTxt.Close();

            trim_alpha_proc.StartInfo.Arguments = "/a:" + _maxAlphaValue.ToString();
            if (_alphaEdgeCorrection == true)
            {
                trim_alpha_proc.StartInfo.Arguments += " /i";
            }
            trim_alpha_proc.StartInfo.Arguments += " input.txt output.txt";
            trim_alpha_proc.Start();
            trim_alpha_proc.BeginOutputReadLine();
            trim_alpha_proc.BeginErrorReadLine();
            trim_alpha_proc.WaitForExit();
            if (trim_alpha_proc.ExitCode != 0)
            {
                return false;
            }

            System.IO.StreamReader outputTxt = new System.IO.StreamReader("output.txt");
            string line = outputTxt.ReadLine();
            while (line != null)
            {
                string[] lineSplit = line.Split(' ');
                if (_resImages.imageNodeDict.ContainsKey(lineSplit[0]) == false)
                {
                    line = outputTxt.ReadLine();
                    continue;
                }
                System.Collections.ArrayList resNodeList = _resImages.imageNodeDict[lineSplit[0]];
                foreach (XmlNode resNode in resNodeList)
                {
                    XmlNodeList childs = resNode.ChildNodes;
                    foreach (XmlNode child in childs)
                    {
                        if (child.Name != "File")
                        {
                            continue;
                        }
                        XmlNode pathNode = child.Attributes.GetNamedItem("Path");
                        if (pathNode != null
                            && pathNode.Value == lineSplit[0])
                        {
                            XmlAttribute attr = _resImages.resourceXml.CreateAttribute("Offset");
                            attr.Value = lineSplit[1] + ";" + lineSplit[2];
                            child.Attributes.Append(attr);
                            attr = _resImages.resourceXml.CreateAttribute("MaxSize");
                            attr.Value = lineSplit[3] + ";" + lineSplit[4];
                            child.Attributes.Append(attr);
                        }

                    }
                }
                line = outputTxt.ReadLine();
            }
            outputTxt.Close();

            // remove input.txt and output.txt
            System.IO.File.Delete("input.txt");
            System.IO.File.Delete("output.txt");

            return true;
        }

        private bool mne_convert(ResourceImages _resImages, decimal _jpegQuality)
        {
            if (System.IO.File.Exists(m_utilsPath + System.IO.Path.DirectorySeparatorChar + "MNEConvertTool.exe") == false)
            {
                logMessage("MNEConvertTool.exe not found", Color.Red);
                return false;
            }
            System.Diagnostics.Process mne_convert_proc = new System.Diagnostics.Process();
            mne_convert_proc.StartInfo.FileName = m_utilsPath + System.IO.Path.DirectorySeparatorChar + "MNEConvertTool";
            mne_convert_proc.StartInfo.UseShellExecute = false;
            mne_convert_proc.StartInfo.RedirectStandardOutput = true;
            mne_convert_proc.StartInfo.RedirectStandardError = true;
            mne_convert_proc.StartInfo.CreateNoWindow = true;
            mne_convert_proc.EnableRaisingEvents = true;
            mne_convert_proc.ErrorDataReceived += new System.Diagnostics.DataReceivedEventHandler(proccess_ErrorDataReceived);
            mne_convert_proc.OutputDataReceived += new System.Diagnostics.DataReceivedEventHandler(proccess_OutputDataReceived);


            System.IO.StreamWriter inputTxt = new System.IO.StreamWriter("input.txt");
            foreach (string filePath in _resImages.imageNodeDict.Keys)
            {    
                bool convertToJPEG = true;
                foreach (XmlNode resourceNode in _resImages.imageNodeDict[filePath])
                {
                    XmlNode xmlNoJPEG = resourceNode.SelectSingleNode("*[@Path=\"" + filePath + "\"]/@NoJPEG");
                    if (xmlNoJPEG != null
                        && (xmlNoJPEG.Value == "1" || xmlNoJPEG.Value.ToLower() == "true"))
                    {
                        convertToJPEG = false;
                        break;
                    }
                }
                if (convertToJPEG == true)
                {
                    inputTxt.WriteLine(filePath);
                }
            }
            inputTxt.Close();

            mne_convert_proc.StartInfo.Arguments = "/q:" + _jpegQuality.ToString()
            + " input.txt output.txt";
            mne_convert_proc.Start();
            mne_convert_proc.BeginOutputReadLine();
            mne_convert_proc.BeginErrorReadLine();
            mne_convert_proc.WaitForExit();
            if (mne_convert_proc.ExitCode != 0)
            {
                return false;
            }

            System.IO.StreamReader outputTxt = new System.IO.StreamReader("output.txt");
            string line = outputTxt.ReadLine();
            while (line != null)
            {
                string[] lineSplit = line.Split(' ');
                string toDelete = "";
                if (lineSplit[0] != lineSplit[1])
                {
                    System.IO.FileInfo info0 = new System.IO.FileInfo(lineSplit[0]);
                    System.IO.FileInfo info1 = new System.IO.FileInfo(lineSplit[1]);
                    if (info0.Length > info1.Length)
                    {
                        toDelete = lineSplit[0];

                    }
                    else
                    {
                        toDelete = lineSplit[1];
                    }
                    System.IO.File.Delete(toDelete);
                }
                if (toDelete == lineSplit[0])
                {
                    System.Collections.ArrayList resNodeList = _resImages.imageNodeDict[lineSplit[0]];
                    _resImages.imageNodeDict.Remove(lineSplit[0]);
                    if (_resImages.imageNodeDict.ContainsKey(lineSplit[1]) == false)
                    {
                        _resImages.imageNodeDict.Add(lineSplit[1], resNodeList);
                    }
                    else
                    {
                        _resImages.imageNodeDict[lineSplit[1]].AddRange(resNodeList);
                    }
                    foreach (XmlNode resNode in resNodeList)
                    {
                        XmlNodeList childs = resNode.ChildNodes;
                        foreach (XmlNode child in childs)
                        {
                            if (child.Name != "File"
                                 && child.Name != "Image")
                            {
                                continue;
                            }
                            XmlNode pathNode = child.Attributes.GetNamedItem("Path");
                            if (pathNode != null
                                && pathNode.Value == lineSplit[0])
                            {
                                pathNode.Value = lineSplit[1];
                            }

                        }
                    }
                }
                line = outputTxt.ReadLine();
            }
            outputTxt.Close();

            // remove input.txt and output.txt
            System.IO.File.Delete("input.txt");
            System.IO.File.Delete("output.txt");

            return true;
        }

        private void patchWin32(string _binaryFile, string _iconFile, string _projectName)
        {
            System.Diagnostics.Process upx_proc = new System.Diagnostics.Process();
            upx_proc.StartInfo.FileName = m_utilsPath + System.IO.Path.DirectorySeparatorChar + "upx"
                + System.IO.Path.DirectorySeparatorChar + "upx.exe";
            upx_proc.StartInfo.Arguments = "-d " + _binaryFile;
            upx_proc.StartInfo.UseShellExecute = false;
            upx_proc.StartInfo.RedirectStandardOutput = true;
            upx_proc.StartInfo.RedirectStandardError = true;
            upx_proc.StartInfo.CreateNoWindow = true;
            upx_proc.EnableRaisingEvents = true;
            upx_proc.ErrorDataReceived += new System.Diagnostics.DataReceivedEventHandler(proccess_ErrorDataReceived);
            upx_proc.OutputDataReceived += new System.Diagnostics.DataReceivedEventHandler(proccess_OutputDataReceived);
            upx_proc.Start();
            upx_proc.BeginOutputReadLine();
            upx_proc.BeginErrorReadLine();
            upx_proc.WaitForExit();
            upx_proc.CancelOutputRead();
            upx_proc.CancelErrorRead();

            System.Diagnostics.Process res_hack_proc = new System.Diagnostics.Process();
            res_hack_proc.StartInfo.FileName = m_utilsPath + System.IO.Path.DirectorySeparatorChar + "ResHacker"
                + System.IO.Path.DirectorySeparatorChar + "ResHacker.exe";
            res_hack_proc.StartInfo.UseShellExecute = false;
            res_hack_proc.StartInfo.RedirectStandardOutput = true;
            res_hack_proc.StartInfo.RedirectStandardError = true;
            res_hack_proc.StartInfo.CreateNoWindow = true;
            res_hack_proc.EnableRaisingEvents = true;
            res_hack_proc.ErrorDataReceived += new System.Diagnostics.DataReceivedEventHandler(proccess_ErrorDataReceived);
            res_hack_proc.OutputDataReceived += new System.Diagnostics.DataReceivedEventHandler(proccess_OutputDataReceived);
            if (_iconFile != "")
            {
                res_hack_proc.StartInfo.Arguments = "-modify " + _binaryFile + ", " + _binaryFile +
                    ", " + _iconFile + ",ICONGROUP, 100,";
                res_hack_proc.Start();
                res_hack_proc.BeginOutputReadLine();
                res_hack_proc.BeginErrorReadLine();
                res_hack_proc.WaitForExit();
                res_hack_proc.CancelOutputRead();
                res_hack_proc.CancelErrorRead();
            }
            if (_projectName != "")
            {
                System.IO.StreamWriter projWriter = new System.IO.StreamWriter("reshacker.txt");
                projWriter.Write('/' + m_companyName + '/' + _projectName);
                projWriter.Close();
                res_hack_proc.StartInfo.Arguments = "-add " + _binaryFile + ", " + _binaryFile +
                    ", reshacker.txt, RCDATA, 101,";
                res_hack_proc.Start();
                res_hack_proc.BeginOutputReadLine();
                res_hack_proc.BeginErrorReadLine();
                res_hack_proc.WaitForExit();
                System.IO.File.Delete("reshacker.txt");
            }

            upx_proc.StartInfo.Arguments = "-9 " + _binaryFile;
            upx_proc.Start();
            upx_proc.BeginOutputReadLine();
            upx_proc.BeginErrorReadLine();
            upx_proc.WaitForExit();

            System.IO.File.Move(_binaryFile, _projectName.Replace( ':', '.') + ".exe");
        }

        private void unwideFileNames(XmlDocument _xmlDoc)
        {
            XmlNodeList filePathNodeList = _xmlDoc.SelectNodes("/DataBlock/Resource[@Type='ResourceImageDefault']/File[@Path][@From][@To][@Step]");
            foreach( XmlNode filePathNode in filePathNodeList )
            {
                XmlNode filePathParent = filePathNode.ParentNode;
                string path = filePathNode.SelectSingleNode("@Path").Value;
                string from = filePathNode.SelectSingleNode("@From").Value;
                string to = filePathNode.SelectSingleNode("@To").Value;
                string step = filePathNode.SelectSingleNode("@Step").Value;

                int iFrom = -1;
                int iTo = -1;
                int iStep = -1;
                iFrom = System.Convert.ToInt32(from);
                iTo = System.Convert.ToInt32(to);
                iStep = System.Convert.ToInt32(step);

                if (iFrom >= 0 && iTo >= 0)
                {
                    filePathParent.RemoveChild( filePathNode );
                    StringBuilder fname = new StringBuilder(path.Length * 2);
                    if (iStep > 0)
                    {
                        for (int i = iFrom; i <= iTo; i += iStep)
                        {
                            API.sprintf(fname, path, __arglist(i));
                            string imagePath = fname.ToString();
                            XmlAttribute attr = _xmlDoc.CreateAttribute("Path");
                            attr.Value = imagePath;
                            filePathParent.AppendChild(_xmlDoc.CreateElement("File"))
                                .Attributes.Append(attr);
                        }
                    }
                    else if (iStep < 0)
                    {
                        for (int i = iFrom; i >= iTo; i += iStep)
                        {
                            API.sprintf(fname, path, __arglist(i));
                            string imagePath = fname.ToString();
                            XmlAttribute attr = _xmlDoc.CreateAttribute("Path");
                            attr.Value = imagePath;
                            filePathParent.AppendChild(_xmlDoc.CreateElement("File"))
                                .Attributes.Append(attr);
                        }
                    }
                }

            }
        }

        private void checkRegister(XmlDocument _xmlDoc)
        {
            string[] nodeAttribPairs = { "File/@Path", "Fontdef/@Path", "Image/@Path", "Background/@Image",
                "LeftTop/@Image", "Left/@Image", "Top/@Image", "RightTop/@Image", "Right/@Image",
                "RightBottom/@Image", "Bottom/@Image", "LeftBottom/@Image" };
            string searchString = "/DataBlock/Resource/" + nodeAttribPairs[0];
            for (int i = 1; i < nodeAttribPairs.Length; ++i)
            {
                searchString += " | /DataBlock/Resource/" + nodeAttribPairs[i];
            }
            XmlNodeList filePathNodes = _xmlDoc.SelectNodes(searchString);
            foreach (XmlNode filePathNode in filePathNodes)
            {
                string filepath = filePathNode.Value;
                if (filepath != "CreateTarget" && filepath != "CreateImage"
                    && Utils.checkPathRegister(filepath) == false)
                {
                    logMessage("Error: Register mismatch (" + filepath + ")\n", Color.Red);
                }
                else if (filepath.IndexOf(' ') != -1)
                {
                    logMessage("Ahtung!!! Whitespaces!!!:" + filepath + '\n', Color.Red);
                }

            }
        }

        private void makeLowerRegister(XmlDocument _xmlDoc)
        {
            string[] nodeAttribPairs = { "File/@Path", "Fontdef/@Path", "Image/@Path", "Background/@Image",
                "LeftTop/@Image", "Left/@Image", "Top/@Image", "RightTop/@Image", "Right/@Image",
                "RightBottom/@Image", "Bottom/@Image", "LeftBottom/@Image" };
            string searchString = "/DataBlock/Resource/" + nodeAttribPairs[0];
            for (int i = 1; i < nodeAttribPairs.Length; ++i)
            {
                searchString += " | /DataBlock/Resource/" + nodeAttribPairs[i];
            }
            XmlNodeList filePathNodes = _xmlDoc.SelectNodes(searchString);
            foreach (XmlNode filePathNode in filePathNodes)
            {
                string filepath = filePathNode.Value;
                if (filepath != "CreateTarget" && filepath != "CreateImage")
                {
                    if (System.IO.File.Exists(filepath) == false)
                    {
                        logMessage("Error: File not found (" + filepath + ")\n", Color.Red);
                    }
                    else
                    {
                        string filename = System.IO.Path.GetFileName(filepath.Replace('/','\\')).ToLower();
                        string before_filename = filepath.Substring(0, filepath.Length - filename.Length);
                        filePathNode.Value = before_filename + filename;
                        System.IO.File.Move(filepath, filePathNode.Value);
                    }
                }

            }

        }

    }


}