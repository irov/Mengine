using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.IO;

namespace Editor
{
    static class Program
    {
        static public byte[] GetAssemblyAsBytes(String path)
        {
            if (!File.Exists(path))
            {
                throw new FileNotFoundException("Unable to load assembly. File not found", path);
            }
            try
            {
                FileInfo file = new FileInfo(path);
                byte[] bytes = new byte[file.Length];
                FileStream stream = new FileStream(path, FileMode.Open);
                stream.Read(bytes, 0, bytes.Length);
                stream.Close();
                return bytes;
            }
            catch (Exception ex)
            {
                throw new Exception("Unable to read assembly.", ex);
            }
        }

        static public void LoadAssemblyByFileName(String file, AppDomain domain)
        {
            byte[] bytes = GetAssemblyAsBytes(file);
            if (bytes.Length == 0)
            {
                throw new Exception("Unable to load assembly. Length of file was 0.");
            }
            domain.Load(bytes);
        }

        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new Form1());
        }
    }
}