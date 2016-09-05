﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace pCarsAPI_Demo
{
    public partial class pCarsDataClass : INotifyPropertyChanged
    {

        private ePitMode mpitmode;                           // [ enum (Type#7) Pit Mode ]
        private ePitSchedule mpitschedule;                       // [ enum (Type#8) Pit Stop Schedule ]

        public ePitMode mPitMode
        {
            get
            {
                return mpitmode;
            }
            set
            {
                if (mpitmode == value)
                    return; 
                SetProperty(ref mpitmode, value);
            }
        }// [ enum (Type#6) Flag Reason ]

        public ePitSchedule mPitSchedule
        {
            get
            {
                return mpitschedule;
            }
            set
            {
                if (mpitschedule == value)
                    return; 
                SetProperty(ref mpitschedule, value);
            }
        }// [ enum (Type#6) Flag Reason ]
    }
}
