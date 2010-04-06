using System;
using System.Collections.Generic;
using System.Diagnostics;

namespace MengeSourceBuilder
{
    class JobSVNExport : Job
    {
        private string m_svnToolPath;
        private string m_url;
        private string m_path;
        private Process m_svnProcess = new Process();
        private bool m_svnProcessRunning = false;

        public JobSVNExport( string _svnToolPath, string _url, string _path )
        {
            m_svnToolPath = _svnToolPath;
            m_url = _url;
            m_path = _path;

            m_svnProcess.StartInfo.FileName = m_svnToolPath;
            m_svnProcess.StartInfo.Arguments = "export --force " + m_url + " " + m_path;
            m_svnProcess.StartInfo.UseShellExecute = false;
            m_svnProcess.StartInfo.RedirectStandardOutput = true;
            m_svnProcess.StartInfo.RedirectStandardError = true;
            m_svnProcess.StartInfo.CreateNoWindow = true;
            m_svnProcess.EnableRaisingEvents = true;
            m_svnProcess.ErrorDataReceived += new System.Diagnostics.DataReceivedEventHandler(this.onErrorDataReceived_);
            m_svnProcess.OutputDataReceived += new System.Diagnostics.DataReceivedEventHandler(this.onOutputDataReceived_);
        }

        public override void jobImpl_()
        {
            m_svnProcessRunning = true;
            m_svnProcess.Start();
            m_svnProcess.BeginOutputReadLine();
            m_svnProcess.BeginErrorReadLine();
            m_svnProcess.WaitForExit();
            m_svnProcessRunning = false;
            m_progress = 1.0f;
        }

        public override void abortImpl_()
        {
            if (m_svnProcessRunning == true)
            {
                m_svnProcessRunning = false;
                m_svnProcess.Kill();
            }
        }

        private void onErrorDataReceived_( object o, DataReceivedEventArgs _args )
        {
            
        }

        private void onOutputDataReceived_( object o, DataReceivedEventArgs _args )
        {

        }
    }
}
