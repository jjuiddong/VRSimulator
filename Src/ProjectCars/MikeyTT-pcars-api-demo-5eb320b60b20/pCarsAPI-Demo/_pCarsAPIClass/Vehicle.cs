using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace pCarsAPI_Demo
{
    public partial class pCarsDataClass : INotifyPropertyChanged
    {
        // Vehicle & Track information

        private string mcarname;                
        private string mcarclassname;           

        public string mCarName
        {
            get
            {
                return mcarname;
            }
            set
            {
                if (mcarname == value)
                    return; 
                SetProperty(ref mcarname, value);
            }
        }

        public string mCarClassName
        {
            get
            {
                return mcarclassname;
            }
            set
            {
                if (mcarclassname == value)
                    return; 
                SetProperty(ref mcarclassname, value);
            }
        }
    }
}
