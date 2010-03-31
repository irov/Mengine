using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace MengeTextEditor
{
    public partial class FindNext : Form
    {
        public delegate void FindCallback(string findMessage, bool matchCase);

        private FindCallback m_findCallback = null;

        public FindNext( FindCallback _findCallback )
        {
            m_findCallback = _findCallback;
            InitializeComponent();
        }

        private void m_findBtn_Click(object sender, EventArgs e)
        {
            if (m_findCallback != null)
            {
                m_findCallback(m_findTextEdit.Text, m_matchCaseChk.Checked);
            }
        }

        private void FindNext_FormClosing(object sender, FormClosingEventArgs e)
        {
            e.Cancel = true;
            this.Hide();
        }

        private void FindNext_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Escape)
            {
                this.Hide();
            }
        }

        private void m_findTextEdit_KeyDown(object sender, KeyEventArgs e)
        {
            FindNext_KeyDown(sender, e);
        }
    }
}