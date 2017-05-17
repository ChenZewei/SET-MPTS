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
#include "rta_gdc_native.h"
#include "rta_pdc_ro.h"
#include "rta_gfp_czw.h"

SchedTestBase* SchedTestFactory::createSchedTest(string test_name, TaskSet& tasks, ProcessorSet& processors, ResourceSet& resources)
{
	if(0 == strcmp(test_name.data(), "RTA-GFP-native"))
	{
		tasks.RM_Order();
		return new RTA_GFP_native(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-GFP-BC"))
	{
		tasks.RM_Order();
		return new RTA_GFP_BC(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-GFP-NG"))
	{
		tasks.RM_Order();
		return new RTA_GFP_NG(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-PFP-WF"))
	{
		tasks.RM_Order();
		return new RTA_PFP_WF(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-PFP-WF-spinlock"))
	{
		tasks.RM_Order();
		return new RTA_PFP_WF_spinlock(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-PDC-WF-spinlock"))
	{
		tasks.DC_Order();
		return new RTA_PFP_WF_spinlock(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-PFP-WF-semaphore"))
	{
		tasks.RM_Order();
		return new RTA_PFP_WF_semaphore(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-PDC-WF-semaphore"))
	{
		tasks.DC_Order();
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
	else if(0 == strcmp(test_name.data(), "RTA-GRM"))
	{
		tasks.RM_Order();
		return new RTA_GFP_NG(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-GDM-NG"))
	{
		tasks.DM_Order();
		return new RTA_GFP_NG(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-GDC-NG"))
	{
		tasks.DC_Order();
		return new RTA_GFP_NG(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-GSMP-NG"))
	{
		tasks.SM_PLUS_Order();
		return new RTA_GFP_NG(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-GSMP2-NG"))
	{
		tasks.SM_PLUS_2_Order();
		return new RTA_GFP_NG(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-GSMP3-NG"))
	{
		tasks.SM_PLUS_3_Order();
		return new RTA_GFP_NG(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-GSMP4-NG"))
	{
		tasks.SM_PLUS_4_Order(processors.get_processor_num());
		return new RTA_GFP_NG(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-GLO-NG"))
	{
		tasks.Leisure_Order();
		return new RTA_GFP_NG(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-GDCC"))
	{
		tasks.DCC_Order();
		return new RTA_GFP_NG(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-PDC-WF"))
	{
		tasks.DC_Order();
		return new RTA_PFP_WF(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-GDC-native"))
	{
		tasks.DC_Order();
		return new RTA_GDC_native(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-GDCC-native"))
	{
		tasks.DCC_Order();
		return new RTA_GDC_native(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-GDDC-native"))
	{
		tasks.DDC_Order();
		return new RTA_GDC_native(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-GUDC-native"))
	{
		tasks.UDC_Order();
		return new RTA_GDC_native(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-GSMP-native"))
	{
		tasks.SM_PLUS_Order();
		return new RTA_GDC_native(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-GDC-BC"))
	{
		tasks.DC_Order();
		return new RTA_GFP_BC(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-GDDC-BC"))
	{
		tasks.DDC_Order();
		return new RTA_GFP_BC(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-GSMP-BC"))
	{
		tasks.SM_PLUS_Order();
		return new RTA_GFP_BC(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-GSMP2-BC"))
	{
		tasks.SM_PLUS_2_Order();
		return new RTA_GFP_BC(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-GSMP3-BC"))
	{
		tasks.SM_PLUS_3_Order();
		return new RTA_GFP_BC(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-GSMP4-BC"))
	{
		tasks.SM_PLUS_4_Order(processors.get_processor_num());
		return new RTA_GFP_BC(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-PDC-RO"))
	{
		return new RTA_PDC_RO(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-GFP-CZW"))
	{
		tasks.RM_Order();
		return new RTA_GFP_CZW(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-GSMP4-CZW"))
	{
		tasks.SM_PLUS_4_Order(processors.get_processor_num());
		return new RTA_GFP_CZW(tasks, processors, resources);
	}
	else
		return NULL;
}
