﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace spiked3.winViz
{
    /// <summary>
    /// Interaction logic for ImportDlg.xaml
    /// </summary>
    public partial class ImportDlg : Window
    {
        public ImportDlg()
        {
            InitializeComponent();
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            Microsoft.Win32.FileDialog d = new Microsoft.Win32.OpenFileDialog();
            d.ShowDialog();
        }
    }
}
