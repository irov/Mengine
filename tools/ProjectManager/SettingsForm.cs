using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace ProjectManager
{
    public partial class SettingsForm : Form
    {
        public bool sound
        {
            get
            { return _sound; }
        }

        public bool debuginfo
        {
            get
            { return _debuginfo; }
        }

        public bool particles
        {
            get
            { return _particles; }
        }

        private bool _sound = false;
        private bool _debuginfo = false;
        private bool _particles = false;

        public SettingsForm()
        {
            InitializeComponent();
        }

        private void OkButton_Click(object sender, EventArgs e)
        {
            _sound = SoundCheckBox.Checked;
            _debuginfo = DebugInfoCheckBox.Checked;
            _particles = ParticlesCheckBox.Checked;

            this.DialogResult = DialogResult.OK;
        }
    }
}