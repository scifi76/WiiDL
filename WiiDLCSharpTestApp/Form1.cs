using System;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace WiiDLCSharpTestApp
{
    public partial class Form1 : Form
    {
        [DllImport("WiiDLBase.dll")]
        static extern bool Open(string isoFileName);

        public Form1()
        {
            InitializeComponent();
        }

        private void btnTest_Click(object sender, EventArgs e)
        {
            Open(@"C:\Test.iso");
        }
    }
}
