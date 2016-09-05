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
        // Session Type

        private eGameState mgamestate = eGameState.GAME_EXITED;                         // [ enum (Type#1) Game state ]
        private eSessionState msessionstate = eSessionState.SESSION_INVALID;            // [ enum (Type#2) Session state ]
        private eRaceState mracestate;                                                  // [ RANGE = 0->... ]

        public eGameState mGameState
        {
            get
            {
                return mgamestate;
            }
            set
            {
                if (mgamestate == value)
                    return;
                SetProperty(ref mgamestate, value);
            }
        }

        public eSessionState mSessionState
        {
            get
            {
                return msessionstate;
            }
            set
            {
                if (msessionstate == value)
                    return;
                SetProperty(ref msessionstate, value);
            }
        }

        public eRaceState mRaceState
        {
            get
            {
                return mracestate;
            }
            set
            {
                if (mracestate == value)
                    return;
                SetProperty(ref mracestate, value);
            }
        }
    }
}