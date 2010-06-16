using System;
using System.Collections.Generic;
using System.Text;
using System.Net;
using System.Diagnostics;
using System.IO;

namespace MengeSourceBuilder
{
    class JobArchive : Job
    {
        private string m_archiveToolPath;
        private string m_url;
        private string m_path;
        private Process m_archiveProcess = new Process();
        private bool m_archiveProcessRunning = false;
        private string m_oldCD;
        private WebClient m_webClient = new WebClient();

        public JobArchive(string _archiveToolPath, string _url, string _path)
        {
            m_archiveToolPath = _archiveToolPath;
            m_url = _url;
            m_path = _path;

            m_archiveProcess.StartInfo.FileName = m_archiveToolPath;
            m_archiveProcess.StartInfo.UseShellExecute = false;
            m_archiveProcess.StartInfo.RedirectStandardOutput = true;
            m_archiveProcess.StartInfo.RedirectStandardError = true;
            m_archiveProcess.StartInfo.CreateNoWindow = true;
            m_archiveProcess.EnableRaisingEvents = true;
            m_archiveProcess.ErrorDataReceived += new System.Diagnostics.DataReceivedEventHandler(this.onErrorDataReceived_);
            m_archiveProcess.OutputDataReceived += new System.Diagnostics.DataReceivedEventHandler(this.onOutputDataReceived_);
        }

        public override void jobImpl_()
        {
            m_webClient.DownloadProgressChanged += new DownloadProgressChangedEventHandler( this.onDownloadProgressChanged_ );
            string winPath = m_path.Replace( '/', Path.DirectorySeparatorChar );
            DirectoryInfo pathInfo = new DirectoryInfo(winPath);
            if (pathInfo.Exists == false)
            {
                pathInfo.Create();
            }
            string filename = Path.GetFileName(m_url.Replace('/', Path.DirectorySeparatorChar));
            string downloadedFilename = Path.Combine(winPath, filename);
            m_webClient.DownloadFile(m_url, downloadedFilename);

            m_oldCD = Directory.GetCurrentDirectory();
            Directory.SetCurrentDirectory(winPath);
            m_archiveProcessRunning = true;
            m_archiveProcess.StartInfo.Arguments = "x " + downloadedFilename;
            m_archiveProcess.Start();
            m_archiveProcess.BeginOutputReadLine();
            m_archiveProcess.BeginErrorReadLine();
            m_archiveProcess.WaitForExit();
            m_archiveProcessRunning = false;
            Directory.SetCurrentDirectory(m_oldCD);

            m_progress = 1.0f;
        }

        public override void abortImpl_()
        {
            if (m_archiveProcessRunning == true)
            {
                m_archiveProcessRunning = false;
                m_archiveProcess.Kill();
                Directory.SetCurrentDirectory(m_oldCD);
            }
        }

        private void onErrorDataReceived_(object sender, DataReceivedEventArgs _args)
        {
            
        }

        private void onOutputDataReceived_(object sender, DataReceivedEventArgs _args)
        {

        }

        private void onDownloadProgressChanged_(Object sender, DownloadProgressChangedEventArgs _args)
        {
            m_progress = _args.ProgressPercentage * 0.01f;
        }

    }
}
