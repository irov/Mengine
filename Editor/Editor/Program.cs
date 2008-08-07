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
        [STAThread]
        static void Main()
        {

            AppDomain.CurrentDomain.Load("ManagedFramework");
            ManagedFramework framework = new ManagedFramework();
            framework.createWindow(new IntPtr());
        /*    try
            {
                ManagedFramework framework = new ManagedFramework();
                //framework.createWindow(this.Handle);
            }
            catch (Exception ex)
            {
                Console.WriteLine("In Main catch block. Caught: {0}", ex.InnerException);
            }*/

            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new Form1());
            
        }
    }
}