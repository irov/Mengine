using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;

namespace MengeSourceBuilder
{
    abstract class Job
    {
        private bool m_isRunning = false;
        public bool IsRunning
        {
            get
            {
                return m_isRunning;
            }
        }
        protected float m_progress = 0.0f;
        public float Progress
        {
            get
            {
                return m_progress;
            }
        }

        private Thread m_thread = null;
        private bool m_exitSuccessfull = false;

        private void job_()
        {
            try
            {
                jobImpl_();
                m_exitSuccessfull = true;
            }
            catch (ThreadAbortException)
            {
                abortImpl_();
            }
            finally
            {
                m_isRunning = false;
            }
        }

        public void run()
        {
            m_isRunning = true;
            m_thread = new Thread( new ThreadStart(this.job_));
            m_thread.Start();
        }

        public void stop()
        {
            if (m_isRunning == true)
            {
                m_thread.Abort();
                m_isRunning = false;
            }
        }

        public abstract void jobImpl_();
        public abstract void abortImpl_();
    }
}
