using System;
using System.Collections.Generic;
using System.Text;

namespace CSGONET.API
{
    [Flags]
    public enum ConVarFlags
    {  
        None = 0,
        Unregistered = (1 << 0),
        DevelopmentOnly = (1 << 1),
        Notify = (1 << 8),
        Replicated = (1<<13)
    }
}
