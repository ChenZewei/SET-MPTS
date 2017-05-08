#include "sched_test_factory.h"
#include "rta_gfp_native.h"
#include "rta_gfp_bc.h"
#include "rta_gfp_ng.h"
#include "rta_pfp_wf.h"
#include "rta_pfp_wf_spinlock.h"
#include "rta_pfp_wf_semaphore.h"
#include "lp_rta_gfp_pip.h"
#include "lp_rta_gfp_fmlp.h"
#include "lp_rta_pfp_dpcp.h"
#include "lp_rta_pfp_mpcp.h"
#include "rta_pfp_ro.h"
#include "ilp_rta_pfp_spinlock.h"
#include "rta_pfp_gs.h"
#include "nc_gedf_vpr.h"

SchedTestBase* SchedTestFactory::createSchedTest(string test_name, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources)
{
	if(0 == strcmp(test_name.data(), "RTA-GFP-native"))
	{
		return new RTA_GFP_native(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-GFP-BC"))
	{
		return new RTA_GFP_BC(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-GFP-NG"))
	{
		tasks.RM_Order();
		return new RTA_GFP_NG(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-PFP-WF"))
	{
		return new RTA_PFP_WF(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-PFP-WF-spinlock"))
	{
		return new RTA_PFP_WF_spinlock(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-PFP-WF-semaphore"))
	{
		return new RTA_PFP_WF_semaphore(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "LP-RTA-GFP-PIP"))
	{
		return new LP_RTA_GFP_PIP(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "LP-RTA-GFP-FMLP"))
	{
		return new LP_RTA_GFP_FMLP(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "LP-RTA-PFP-DPCP"))
	{
		return new LP_RTA_PFP_DPCP(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "LP-RTA-PFP-MPCP"))
	{
		return new LP_RTA_PFP_MPCP(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-PFP-RO"))
	{
		return new RTA_PFP_RO(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "ILP-RTA-PFP-spinlock"))
	{
		return new ILP_RTA_PFP_spinlock(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-PFP-GS"))
	{
		return new RTA_PFP_GS(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "NC-GEDF-VPR"))
	{
		return new NC_GEDF_VPR(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-GDC"))
	{
		tasks.DC_Order();
		return new RTA_GFP_NG(tasks, processors, resources);
	}
	else
		return NULL;
}
