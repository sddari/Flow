#include "SProcessComMgr.h"
#include "sprocssrun.h"
#include "sview.h"


SProcessComMgr::SProcessComMgr()
{    
}

void SProcessComMgr::Run()
{        
    SProcssRun run;
    Start(m_strCommand);
    run.Run(m_strCommand);
    while (run.IsRunning());
    End(run.GetOutput());
}
