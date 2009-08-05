using System;
using System.IO;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using WiiDLManagedWrapper;
namespace WiiDLCSharpTestApp
{
    public partial class Form1 : Form
    {

        MDisc d;

        public Form1()
        {
            InitializeComponent();
        }

        private void btnLoad_Click(object sender, EventArgs e)
        {
            if (File.Exists(txtSourcePath.Text))
            {
                d = new MDisc(txtSourcePath.Text);
                d.Load(true);
                pgBasic.SelectedObject = d;
                pgImage.SelectedObject = d.Image;
                pgImageHeader.SelectedObject = d.Image.Header;

                int i = 0;
                foreach (MPartition part in d.Image.Partitions)
                {
                    ListViewItem item = new ListViewItem();
                    item.SubItems[0].Text = i.ToString();
                    item.SubItems.Add(part.Type);
                    lvPartitions.Items.Add(item);

                    i++;
                }
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

        private void lvPartitions_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (lvPartitions.SelectedItems.Count > 0)
            {

                pgPartInfo.SelectedObject = d.Image.Partitions[int.Parse(lvPartitions.SelectedItems[0].SubItems[0].Text)];

                lvFiles.Items.Clear();
                int i = 0;
                foreach (MFile f in d.Image.Partitions[int.Parse(lvPartitions.SelectedItems[0].SubItems[0].Text)].Files)
                {
                    ListViewItem item = new ListViewItem();
                    item.SubItems[0].Text = i.ToString();
                    item.SubItems.Add(f.FileName);
                    lvFiles.Items.Add(item);
                    i++;
                }
            }
        }

        private void lvFiles_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (lvFiles.SelectedItems.Count > 0)
            {
                pgFileInfo.SelectedObject = d.Image.Partitions[int.Parse(lvPartitions.SelectedItems[0].SubItems[0].Text)].Files[int.Parse(lvFiles.SelectedItems[0].SubItems[0].Text)];
            }

        }
       
    }
}
