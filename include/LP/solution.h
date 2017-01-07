#ifndef SOLUTION_H
#define SOLUTION_H

#include <types.h>
#include <iostream>
#include <glpk.h>

class LinearExpression;
class LinearProgram;

class GLPKSolution
{
	private:
		glp_prob *glpk;
		const LinearProgram &lp;
		const uint row_num;
		const uint col_num;
		uint coeff_num;
		const bool is_mip;//mixed integer programing
		const uint dir;
		const uint aim;
		int simplex_code;
		bool solved;
		
		void solve(double var_lb, double var_ub);
		void set_objective();
		void set_bounds(double col_lb, double col_ub);
		void set_coefficients();
		void set_column_types();
	public:
		GLPKSolution(const LinearProgram &lp, unsigned int max_var_num, double var_lb = 0.0, double var_ub = 1.0, uint dir = 0, uint aim = 0);
		~GLPKSolution();
		void show_error();
		double get_value(unsigned int var) const;
		double evaluate(const LinearExpression &exp) const;
		bool is_solved() const;
};


#endif
