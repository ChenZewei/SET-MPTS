#include <solution.h>
#include <lp.h>
#include <assert.h>

GLPKSolution::GLPKSolution(const LinearProgram &lp, 
							unsigned int max_var_num, 
							double var_lb, 
							double var_ub,
							uint dir,
							uint aim):
							glpk(glp_create_prob()),
							lp(lp),
							col_num(max_var_num),
							row_num(lp.get_equalities().size() + lp.get_inequalities().size()),
							coeff_num(0),
							is_mip(lp.has_binary_variables() || lp.has_integer_variables()),
	  						solved(false),
							dir(dir),
							aim(aim)
{
	if (col_num)
		solve(var_lb, var_ub);
	else
		// Trivial case: no variables.
		// This can happen if a task set does not
		// contain any shared resources.
		solved = true;
}

GLPKSolution::~GLPKSolution()
{
	glp_delete_prob(glpk);
}

void GLPKSolution::show_error()
{
	if (!solved)
	{
		std::cerr << "NOT SOLVED => status: "
			<< (is_mip?glp_mip_status(glpk):glp_get_status(glpk))
			<< " (";
		switch (is_mip?glp_mip_status(glpk):glp_get_status(glpk))
		{
			case GLP_OPT:
				std::cerr << "GLP_OPT";
				break;
			case GLP_FEAS:
				std::cerr << "GLP_FEAS";
				break;
			case GLP_INFEAS:
				std::cerr << "GLP_INFEAS";
				break;
			case GLP_NOFEAS:
				std::cerr << "GLP_NOFEAS";
				break;
			case GLP_UNBND:
				std::cerr << "GLP_UNBND";
				break;
			case GLP_UNDEF:
				std::cerr << "GLP_UNDEF";
				break;
			default:
				std::cerr << "???";
		}
		if(!is_mip)
		{
			std::cerr << ") simplex: " << simplex_code << " (";
			switch (glp_get_status(glpk))
			{
				case GLP_EBADB:
					std::cerr << "GLP_EBADB";
					break;
				case GLP_ESING:
					std::cerr << "GLP_ESING";
					break;
				case GLP_ECOND:
					std::cerr << "GLP_ECOND";
					break;
				case GLP_EBOUND:
					std::cerr << "GLP_EBOUND";
					break;
				case GLP_EFAIL:
					std::cerr << "GLP_EFAIL";
					break;
				case GLP_EOBJLL:
					std::cerr << "GLP_EOBJLL";
					break;
				case GLP_EOBJUL:
					std::cerr << "GLP_EOBJUL";
					break;
				case GLP_EITLIM:
					std::cerr << "GLP_EITLIM";
					break;
				case GLP_ENOPFS:
					std::cerr << "GLP_ENOPFS";
					break;
				case GLP_ENODFS:
					std::cerr << "GLP_ENODFS";
					break;
				default:
					std::cerr << "???";
			}
		}

		std::cerr << ")" << std::endl;
	}
}

double GLPKSolution::get_value(unsigned int var) const
{
	if (is_mip)
		return glp_mip_col_val(glpk, var + 1);
	else
		return glp_get_col_prim(glpk, var + 1);
}

double GLPKSolution::evaluate(const LinearExpression &exp) const
	{
		double sum = 0;
		foreach(exp.get_terms(), term)
		{
			double coeff     = term->first;
			unsigned int var = term->second;
			sum += coeff * get_value(var);
		}
		return sum;
	}

bool GLPKSolution::is_solved() const
{
	return solved;
}
void GLPKSolution::solve(double var_lb, double var_ub)
{
//cout<<"111"<<endl;
	glp_term_out(GLP_OFF);
	switch(dir)
	{
		case 0:
			glp_set_obj_dir(glpk, GLP_MAX);
			break;
		case 1:
			glp_set_obj_dir(glpk, GLP_MIN);
			break;
		default:
			glp_set_obj_dir(glpk, GLP_MAX);
	}
	glp_add_cols(glpk, col_num);
//cout<<row_num<<endl;
	glp_add_rows(glpk, row_num);
//cout<<"222"<<endl;
	set_objective();
//cout<<"333"<<endl;
	set_bounds(var_lb, var_ub);
//cout<<"444"<<endl;
	set_coefficients();
	if (is_mip)
		set_column_types();
//cout<<"555"<<endl;
	if (is_mip)
	{
		glp_iocp glpk_params;
//cout<<"666"<<endl;
		glp_init_iocp(&glpk_params);
//cout<<"777"<<endl;
		// presolver is required because otherwise
		// GLPK expects glpk to hold an optimal solution
		// to the relaxed LP.
		glpk_params.presolve = GLP_ON;
//cout<<"888"<<endl;
		if(0 == aim)
			solved = glp_intopt(glpk, &glpk_params) == 0 && glp_mip_status(glpk) == GLP_OPT;
		else if(1 == aim)
		{
			solved = glp_intopt(glpk, &glpk_params) == 0 && glp_mip_status(glpk) == GLP_FEAS;
cout<<"mip status:"<<glp_mip_status(glpk)<<endl;
		}
	}
	else
	{
		glp_smcp glpk_params;
//cout<<"666"<<endl;
		glp_init_smcp(&glpk_params);
//cout<<"777"<<endl;
		/* Set solver options. The presolver is essential. The other two
		 * options seem to make the solver slightly faster.
		 *
		 * Tested with GLPK 4.43 on wks-50-12.
		 */
		glpk_params.presolve = GLP_ON;
		glpk_params.pricing  = GLP_PT_STD;
		glpk_params.r_test   = GLP_RT_STD;
//cout<<"888"<<endl;
		simplex_code = glp_simplex(glpk, &glpk_params);
//cout<<"999"<<endl;
		if(0 == aim)
			solved = simplex_code == 0 && glp_get_status(glpk) == GLP_OPT;
		else if(1 == aim)
			solved = simplex_code == 0 && glp_get_status(glpk) == GLP_FEAS;
	}
//cout<<"100000"<<endl;
}

void GLPKSolution::set_objective()
{
	assert(lp.get_objective()->get_terms().size() <= col_num);

	foreach (lp.get_objective()->get_terms(), term)
		glp_set_obj_coef(glpk, term->second + 1, term->first);
}

/*
	GLP_FR -8 < x < +8 Free variable
	GLP_LO lb < x < +8 Variable with lower bound
	GLP_UP -8 < x < ub Variable with upper bound
	GLP_DB lb <=x<= ub Double-bound variable
	GLP_FX lb = x = ub Fixed variable
*/
void GLPKSolution::set_bounds(double col_lb, double col_ub)
{
	unsigned int r = 1;

	foreach(lp.get_equalities(), equ)
	{
		glp_set_row_bnds(glpk, r++, GLP_FX, equ->second, equ->second);
		coeff_num += equ->first->get_terms().size();
	}

	foreach(lp.get_inequalities(), inequ)
	{
		glp_set_row_bnds(glpk, r++, GLP_UP, 0, inequ->second);
		coeff_num += inequ->first->get_terms().size();
	}

	for (unsigned int c = 1; c <= col_num; c++)
		glp_set_col_bnds(glpk, c, GLP_DB, col_lb, col_ub);

	foreach(lp.get_non_default_variable_ranges(), bnds)
	{
		unsigned int c = bnds->var_index + 1;
		int col_type;
		col_lb = bnds->lower_bound;
		col_ub = bnds->upper_bound;

		if (bnds->has_upper && bnds->has_lower)
			col_type = GLP_DB;
		else if (!bnds->has_upper && !bnds->has_lower)
			col_type = GLP_FR;
		else if (bnds->has_upper)
			col_type = GLP_UP;
		else
			col_type = GLP_LO;
//cout<<"var_id:"<<bnds->var_index<<" lb:"<<col_lb<<" ub:"<<col_ub<<endl;
		glp_set_col_bnds(glpk, c, col_type, col_lb, col_ub);
	}
}

/*
Construction of coefficient matrix and load it.
*/
void GLPKSolution::set_coefficients()
{
	int *row_idx, *col_idx;
	double *coeff;

	row_idx = new int[1 + coeff_num];
	col_idx = new int[1 + coeff_num];
	coeff   = new double[1 + coeff_num];

	unsigned int r = 1, k = 1;

	foreach(lp.get_equalities(), equ)
	{
		foreach(equ->first->get_terms(), term)
		{
			assert(k <= coeff_num);

			row_idx[k] = r;
			col_idx[k] = 1 + term->second;
			coeff[k]   = term->first;

			k += 1;
		}
		r += 1;
	}


	foreach(lp.get_inequalities(), inequ)
	{
		foreach(inequ->first->get_terms(), term)
		{
			assert(k <= coeff_num);

			row_idx[k] = r;
			col_idx[k] = 1 + term->second;
			coeff[k]   = term->first;

			k += 1;
		}
		r += 1;
	}
/*
for(uint i = 1; i <= coeff_num; i++)
{
	cout<<"["<<row_idx[i]<<","<<col_idx[i]<<"]"<<" = "<<coeff[i]<<endl;
}
*/
	glp_load_matrix(glpk, coeff_num, row_idx, col_idx, coeff);

	delete[] row_idx;
	delete[] col_idx;
	delete[] coeff;
}

void GLPKSolution::set_column_types()
{
	unsigned int col_idx;

	foreach(lp.get_integer_variables(), var_id)
	{
		col_idx = 1 + *var_id;
		// hack: for integer variables, ignore upper bound for now
		glp_set_col_bnds(glpk, col_idx, GLP_LO, 0, 0);
		glp_set_col_kind(glpk, col_idx, GLP_IV);
	}

	foreach(lp.get_binary_variables(), var_id)
	{
		col_idx = 1 + *var_id;
		glp_set_col_kind(glpk, col_idx, GLP_BV);
	}
}
