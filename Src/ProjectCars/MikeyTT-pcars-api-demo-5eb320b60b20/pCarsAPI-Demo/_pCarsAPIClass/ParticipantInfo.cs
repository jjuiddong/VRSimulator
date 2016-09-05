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
        private int mviewedparticipantindex = -1;
        private int mplayerparticipantindex = -1;
        private int mnumparticipants = 0;
        private ObservableCollection<pCarsDataClass.pCarsParticipantsClass> listparticipantinfo = new ObservableCollection<pCarsDataClass.pCarsParticipantsClass>();

        public int mPlayerParticipantIndex
        {
            get
            {
                return mplayerparticipantindex;
            }
            set
            {
                if (mplayerparticipantindex == value)
                    return;
                SetProperty(ref mplayerparticipantindex, value);
            }
        }

        public int mViewedParticipantIndex
        {
            get
            {
                return mviewedparticipantindex;
            }
            set
            {
                if (mviewedparticipantindex == value)
                    return;
                SetProperty(ref mviewedparticipantindex, value);
            }
        }

        public int mNumParticipants
        {
            get
            {
                return mnumparticipants;
            }
            set
            {
                if (mnumparticipants == value)
                    return; 
                SetProperty(ref mnumparticipants, value);
            }
        }

        public ObservableCollection<pCarsDataClass.pCarsParticipantsClass> listParticipantInfo
        {
            get
            {
                return listparticipantinfo;
            }
            set
            {
                if (listparticipantinfo == value)
                    return; 
                SetProperty(ref listparticipantinfo, value);
            }
        }


    }
}
