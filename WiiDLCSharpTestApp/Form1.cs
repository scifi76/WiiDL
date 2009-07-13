using System;
using System.IO;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace WiiDLCSharpTestApp
{
    public partial class Form1 : Form
    {
        

        public Form1()
        {
            InitializeComponent();
        }

        private void btnLoad_Click(object sender, EventArgs e)
        {
            if (File.Exists(txtSourcePath.Text))
            {
                WiiDLManagedWrapper.ManagedDisc d = new WiiDLManagedWrapper.ManagedDisc(txtSourcePath.Text);
                d.Load(true);
                pgBasic.SelectedObject = d;
                pgImage.SelectedObject = d.Image;


            }
            else
            {
                MessageBox.Show("Please drag an image file into the source file textbox");
            }
        }

        private void txtSourcePath_DragDrop(object sender, DragEventArgs e)
        {
            string[] s = (string[])e.Data.GetData(DataFormats.FileDrop, false);
            if (s.Length > 0)
                ((TextBox)sender).Text = s[0];
        }

        private void txtSourcePath_DragEnter(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(DataFormats.FileDrop))
            {
                e.Effect = DragDropEffects.All;
            }
            else
            {
                e.Effect = DragDropEffects.None;
            }
        }
       
    }
}
