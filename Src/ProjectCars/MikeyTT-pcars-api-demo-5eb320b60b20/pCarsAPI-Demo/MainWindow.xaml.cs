using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Threading;

namespace pCarsAPI_Demo
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {

        public static int STRING_LENGTH_MAX = 64;
        public pCarsDataClass pcarsData = new pCarsDataClass();

        private DispatcherTimer dispatchTimer = new DispatcherTimer();

        public MainWindow()
        {
            InitializeComponent();

            this.DataContext = pcarsData; 

            dispatchTimer = new DispatcherTimer() { Interval = TimeSpan.FromMilliseconds(1) };
            dispatchTimer.Tick += pCarsDataGetterLoop;
            dispatchTimer.Start();

        }


        private void pCarsDataGetterLoop(object sender, EventArgs e)
        {
            var returnTuple = pCarsAPI_GetData.ReadSharedMemoryData();

            //item1 is the true/false indicating a good read or not
            if (returnTuple.Item1)
            {
                //map the data that's read from the struct and map it to the class
                pcarsData = pcarsData.MapStructToClass(returnTuple.Item2, pcarsData);

                //Thread.Sleep(500);
            }
            else
            {
                //throw some kind of error or warning?
            }

        
        
        
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            dispatchTimer.Stop();
        }



    }
}
