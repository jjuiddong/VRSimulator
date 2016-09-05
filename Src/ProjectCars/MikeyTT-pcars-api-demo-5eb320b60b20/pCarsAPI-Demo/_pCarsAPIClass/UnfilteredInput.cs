using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace pCarsAPI_Demo
{
    public partial class pCarsDataClass : INotifyPropertyChanged
    {
        // Unfiltered Input

        private float munfilteredthrottle = 0;                       // [ RANGE = 0.0f->1.0f ]
        private float munfilteredbrake = 0;                          // [ RANGE = 0.0f->1.0f ]
        private float munfilteredsteering = 0;                       // [ RANGE = -1.0f->1.0f ]
        private float munfilteredclutch = 0;                         // [ RANGE = 0.0f->1.0f ]

        public float mUnfilteredThrottle
        {
            get
            {
                return munfilteredthrottle;
            }
            set
            {
                if (munfilteredthrottle == value)
                    return; 
                SetProperty(ref munfilteredthrottle, value);
            }
        }

        public float mUnfilteredBrake
        {
            get
            {
                return munfilteredbrake;
            }
            set
            {
                if (munfilteredbrake == value)
                    return; 
                SetProperty(ref munfilteredbrake, value);
            }
        }

        public float mUnfilteredSteering
        {
            get
            {
                return munfilteredsteering;
            }
            set
            {
                if (munfilteredsteering == value)
                    return; 
                SetProperty(ref munfilteredsteering, value);
            }
        }

        public float mUnfilteredClutch
        {
            get
            {
                return munfilteredclutch;
            }
            set
            {
                if (munfilteredclutch == value)
                    return; 
                SetProperty(ref munfilteredclutch, value);
            }
        }
    }
}