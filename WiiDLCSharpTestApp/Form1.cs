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
        uint _curPartition;

        public Form1()
        {
            InitializeComponent();
            _curPartition = 0;
        }

        private void btnLoad_Click(object sender, EventArgs e)
        {
            lvPartitions.Items.Clear();
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
                _curPartition = uint.Parse(lvPartitions.SelectedItems[0].SubItems[0].Text);
                pgPartInfo.SelectedObject = d.Image.Partitions[(int)_curPartition];


                lvFiles.Nodes.Clear();
                foreach (MFolder folder in d.Image.Partitions[(int)_curPartition].Folders)
                {
                    TreeNode folderNode = new TreeNode(folder.FolderName);
                    folderNode.Tag = folder;
                    AddFolder(folder, folderNode);
                    lvFiles.Nodes.Add(folderNode);
                }
            }
        }

        private void AddFolder(MFolder folder, TreeNode folderNode)
        {
            foreach (MFile file in folder.Files)
            {
                TreeNode curFileNode = new TreeNode(file.FileName);
                curFileNode.Tag = file;
                folderNode.Nodes.Add(curFileNode);
                
            }

            foreach (MFolder curFolder in folder.SubFolders)
            {
                TreeNode curFolderNode = new TreeNode(curFolder.FolderName);
                AddFolder(curFolder, curFolderNode);
                folderNode.Nodes.Add(curFolderNode);
            }
           
        }

        private void tvFiles_AfterSelect(object sender, TreeViewEventArgs e)
        {
            pgFileInfo.SelectedObject = e.Node.Tag;
        }

        private void extractFileToolStripMenuItem_Click(object sender, EventArgs e)
        {
        
            if (lvFiles.SelectedNode != null)
            {
                SaveFileDialog sfd = new SaveFileDialog();
                sfd.FileName = lvFiles.SelectedNode.Text;
                if (sfd.ShowDialog() == DialogResult.OK)
                {
                    MFile file = (MFile)lvFiles.SelectedNode.Tag;
                    if (d.ExtractFile(sfd.FileName, file, true))
                    {
                        MessageBox.Show(string.Format("Successfully extracted {0} to {1}", file.FileName, sfd.FileName), "Success", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    }
                    else
                    {
                        MessageBox.Show(string.Format("Failed to extract {0}", file.FileName), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                }
            }
        }

        private void replaceFileToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (lvFiles.SelectedNode != null)
            {
                OpenFileDialog ofd = new OpenFileDialog();
                ofd.FileName = lvFiles.SelectedNode.Text;
                if (ofd.ShowDialog() == DialogResult.OK)
                {
                    MFile file = (MFile)lvFiles.SelectedNode.Tag;
                    d.ReplaceFile(ofd.FileName, file, true);
                }
            }
        }
    }
}
