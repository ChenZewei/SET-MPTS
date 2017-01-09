#ifndef GEDF_NON_PREEMPT_H
#define GEDF_NON_PREEMPT_H

//Global EDF Non-Preemptive Scheduling
#include <iostream>
#include <assert.h>
#include "types.h"
#include <tasks.h>
#include <resources.h>
#include <processors.h>

vector<ulong> delta(Task& ti, TaskSet& tasks)
{

}

ulong gedf_non_preempt_dbf(Task& ti, Task& tk, ProcessorSet& processors, ResourceSet& resources)
{
	ulong p_i = ti.get_period(), p_k = tk.get_period();
	ulong e_k = tk.get_wcet_non_critical_sections();
	uint p_num = processors.get_processor_num();
	uint r_num = resources.get_resourceset_size();
	return ((p_i - p_k)/(3*p_k) + 1)*((4*p_num + 3*r_num)*e_k)/(2*p_num);
}

ulong gedf_non_preempt_psi(Task& ti, Task& tk, ProcessorSet& processors, ResourceSet& resources, ulong interval)
{
	ulong dbf_i_k = gedf_non_preempt_dbf(ti, tk, processors, resources);

	ulong p_i = ti.get_period(), p_k = tk.get_period();
	ulong e_k = tk.get_wcet_non_critical_sections();
	uint p_num = processors.get_processor_num();
	uint r_num = resources.get_resourceset_size();

	ulong element1 = ((4*p_num + 3*r_num)*e_k)/(2*p_num);
	ulong elemetn2 = max(0, interval - dbf_i_k*(2*p_num*p_i)/((4*p_num + 3*r_num)*e_k));

	return dbf_i_k + min(element1, element2);
}

ulong gedf_non_preempt_N(Task& tk, ProcessorSet& processors, ResourceSet& resources, ulong interval)
{
	ulong p_k = tk.get_period();
	ulong e_k = tk.get_wcet_non_critical_sections();
	uint p_num = processors.get_processor_num();
	uint r_num = resources.get_resourceset_size();

	return (interval + double(p_k/3) - double(((4*p_num + 3*r_num)*e_k)/(2*p_num)))/p_k;
}

ulong gedf_non_preempt_xi(Task& tk, ProcessorSet& processors, ResourceSet& resources, ulong interval)
{
	ulong p_k = tk.get_period();
	ulong e_k = tk.get_wcet_non_critical_sections();
	uint p_num = processors.get_processor_num();
	uint r_num = resources.get_resourceset_size();
	ulong N_k = gedf_non_preempt_N(tk, processors, resources, interval);

	ulong element1 = ((4*p_num + 3*r_num)*e_k)/(2*p_num);
	ulong elemetn2 = interval + double(p_k/3) - double(((4*p_num + 3*r_num)*e_k)/(2*p_num)) - N_k*p_k;

	return N_k*((4*p_num + 3*r_num)*e_k)/(2*p_num) + min(element1, element2);
}

ulong gedf_non_preempt_interference(Task& ti, Task& tk, ProcessorSet& processors, ResourceSet& resources, ulong interval)
{
	ulong p_i = ti.get_period();
	ulong xi_k = gedf_non_preempt_xi(tk, processors, resources, interval);
	ulong psi_i_k = gedf_non_preempt_psi(ti, tk, processors, resources, p_i/3);
	
	ulong element = min(psi_i_k, interval - ((4*p_num + 3*r_num)*e_k)/(2*p_num) + 1);
	
	return min(xi_k, element);
}































#endif
