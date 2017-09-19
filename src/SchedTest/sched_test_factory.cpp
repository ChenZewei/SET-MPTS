#include "sched_test_factory.h"
#include "rta_gfp_native.h"
#include "rta_gfp_bc.h"
#include "rta_gfp_ng.h"
#include "rta_pfp_wf.h"
#include "rta_pfp_ff.h"
#include "rta_pfp_ff_opa.h"
#include "rta_pfp_wf_spinlock.h"
#include "rta_pfp_wf_semaphore.h"
#include "rta_pfp_rop.h"
#include "rta_pfp_rop_heterogeneous.h"
#include "rta_pfp_ro_np.h"
#include "rta_pfp_ro_np_opa.h"
#include "rta_pfp_ro_np_sm.h"
#include "rta_pfp_ro_np_sm_opa.h"
#include "rta_pfp_ro_feasible.h"
#include "nc_lp_ee_vpr.h"
#include "rta_gdc_native.h"
#include "rta_pdc_ro.h"
#include "rta_gfp_czw.h"
#include "rta_pfp_ro_opa.h"
#include "rta_pfp_ro_sm_opa.h"
#include "rta_pfp_ro_sm.h"

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
	else if(0 == strcmp(test_name.data(), "RTA-PFP-FF"))
	{
		tasks.RM_Order();
		return new RTA_PFP_FF(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-PFP-FF-2"))
	{
		tasks.DC_Order();
		return new RTA_PFP_FF(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-PFP-FF-OPA"))
	{
		tasks.RM_Order();
		return new RTA_PFP_FF_OPA(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-PFP-FF-SM-OPA"))
	{
		tasks.DC_Order();
		return new RTA_PFP_FF_OPA(tasks, processors, resources);
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
	else if(0 == strcmp(test_name.data(), "RTA-PFP-ROP"))
	{
		tasks.RM_Order();
		return new RTA_PFP_ROP(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-PFP-ROP-HETEROGENEOUS"))
	{
		tasks.RM_Order();
		return new RTA_PFP_ROP_HETEROGENEOUS(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-PFP-RO-DC"))
	{
		tasks.DC_Order();
		return new RTA_PFP_ROP(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-PFP-RO-SM"))
	{
		tasks.RM_Order();
		return new RTA_PFP_RO_SM(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-PFP-RO-OPA"))
	{
		tasks.RM_Order();
		return new RTA_PFP_RO_OPA(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-PFP-RO-SM-OPA"))
	{
		tasks.RM_Order();
		return new RTA_PFP_RO_SM_OPA(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-PFP-RO-NP"))
	{
		tasks.RM_Order();
		return new RTA_PFP_RO_NP(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-PFP-RO-NP-OPA"))
	{
		tasks.RM_Order();
		return new RTA_PFP_RO_NP_OPA(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-PFP-RO-NP-SM"))
	{
		tasks.RM_Order();
		return new RTA_PFP_RO_NP_SM(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-PFP-RO-NP-SM-OPA"))
	{
		tasks.RM_Order();
		return new RTA_PFP_RO_NP_SM_OPA(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "RTA-PFP-RO-FEASIBLE"))
	{
		return new RTA_PFP_RO_FEASIBLE(tasks, processors, resources);
	}
	else if(0 == strcmp(test_name.data(), "NC-LP-EE-VPR"))
	{
		return new NC_LP_EE_VPR(tasks, processors, resources);
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
