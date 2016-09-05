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
        // Motion & Device Related
        private List<float> morientation;                     // [ UNITS = Euler Angles ]
        private List<float> mlocalvelocity;                   // [ UNITS = Metres per-second ]
        private List<float> mworldvelocity;                   // [ UNITS = Metres per-second ]
        private List<float> mangularvelocity;                 // [ UNITS = Radians per-second ]
        private List<float> mlocalacceleration;               // [ UNITS = Metres per-second ]
        private List<float> mworldacceleration;               // [ UNITS = Metres per-second ]
        private List<float> mextentscentre;                   // [ UNITS = Local Space  X  Y  Z ]

        public List<float> mOrientation
        {
            get
            {
                return morientation;
            }
            set
            {
                if (morientation == value)
                    return;
                SetProperty(ref morientation, value);
            }
        }

        public List<float> mLocalVelocity
        {
            get
            {
                return mlocalvelocity;
            }
            set
            {
                if (mlocalvelocity == value)
                    return;
                SetProperty(ref mlocalvelocity, value);
            }
        }

        public List<float> mWorldVelocity
        {
            get
            {
                return mworldvelocity;
            }
            set
            {
                if (mworldvelocity == value)
                    return;
                SetProperty(ref mworldvelocity, value);
            }
        }

        public List<float> mAngularVelocity
        {
            get
            {
                return mangularvelocity;
            }
            set
            {
                if (mangularvelocity == value)
                    return; 
                SetProperty(ref mangularvelocity, value);
            }
        }

        public List<float> mLocalAcceleration
        {
            get
            {
                return mlocalacceleration;
            }
            set
            {
                if (mlocalacceleration == value)
                    return;
                SetProperty(ref mlocalacceleration, value);
            }
        }

        public List<float> mWorldAcceleration
        {
            get
            {
                return mworldacceleration;
            }
            set
            {
                if (mworldacceleration == value)
                    return; 
                SetProperty(ref mworldacceleration, value);
            }
        }

        public List<float> mExtentsCentre
        {
            get
            {
                return mextentscentre;
            }
            set
            {
                if (mextentscentre == value)
                    return; 
                SetProperty(ref mextentscentre, value);
            }
        }

    
    }
}
