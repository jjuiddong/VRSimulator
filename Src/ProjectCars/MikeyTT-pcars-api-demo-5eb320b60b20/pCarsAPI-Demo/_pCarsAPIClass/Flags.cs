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
        private eFlagColors mflagcolour;
        private eFlagReason mflagreason;

        public eFlagColors mFlagColour
        {
            get
            {
                return mflagcolour;
            }
            set
            {
                if (mflagcolour == value)
                    return; SetProperty(ref mflagcolour, value);
            }
        }// [ enum (Type#5) Flag Colour ]

        public eFlagReason mFlagReason
        {
            get
            {
                return mflagreason;
            }
            set
            {
                if (mflagreason == value)
                    return; SetProperty(ref mflagreason, value);
            }
        }// [ enum (Type#6) Flag Reason ]
    }
}
