using System;
using System.Collections.Generic;
using System.Windows.Forms;

namespace MengeProjectBuilder
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new Form1());
            Properties.Settings.Default.Save();
        }
    }
}