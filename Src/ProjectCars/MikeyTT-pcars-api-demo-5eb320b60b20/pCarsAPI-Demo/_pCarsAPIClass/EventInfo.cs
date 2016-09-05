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

        private uint mlapsinevent;                       // [ RANGE = 0->... ]   [ UNSET = 0 ]
        private string mtracklocation;
        private string mtrackvariant;
        private float mtracklength;

        public uint mLapsInEvent
        {
            get
            {
                return mlapsinevent;
            }
            set
            {
                if (mlapsinevent == value)
                    return; 
                SetProperty(ref mlapsinevent, value);
            }
        }

        public string mTrackLocation
        {
            get
            {
                return mtracklocation;
            }
            set
            {
                if (mtracklocation == value)
                    return;
                SetProperty(ref mtracklocation, value);
            }
        }

        public string mTrackVariant
        {
            get
            {
                return mtrackvariant;
            }
            set
            {
                if (mtrackvariant == value)
                    return;
                SetProperty(ref mtrackvariant, value);
            }
        }

        public float mTrackLength
        {
            get
            {
                return mtracklength;
            }
            set
            {
                if (mtracklength == value)
                    return; 
                SetProperty(ref mtracklength, value);
            }
        }
    }
}
