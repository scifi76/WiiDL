using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace WiiDLTestApp
{
    public partial class Form1 : Form
    {

        // WiiDL Imports
        [DllImport("WiiDL.dll")]
        static extern void Open(string isoPath);

        public Form1()
        {
            InitializeComponent();
        }

        private void txtIsoPath_DragDrop(object sender, DragEventArgs e)
        {            
            string[] s = (string[])e.Data.GetData(DataFormats.FileDrop, false);
            if (s.Length > 0)
                ((TextBox)sender).Text = s[0];
        }

        private void txtIsoPath_DragEnter(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(DataFormats.FileDrop))
                e.Effect = DragDropEffects.All;
            else
                e.Effect = DragDropEffects.None;
        }

        private void btnOpenIso_Click(object sender, EventArgs e)
        {
            Open(txtIsoPath.Text);
        }
    }
}
