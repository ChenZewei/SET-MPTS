#ifndef _LP_H_
#define _LP_H_

#include <type.h>
#include <vector>
#include <set>

#include <math-helper.h>

using namespace std;

typedef pair<double, uint> Term;
typedef vector<Term> Terms;

class LinearExpression
{
	private:
		Terms terms;
	public:
		Terms& get_terms() const
		{
			return terms;
		}
		
		bool has_terms() const
		{
			return !terms.empty();
		}
		int get_terms_size() const
		{
			return terms.size();
		}
		
		void add_term(uint var_index, double coefficient = 1)
		{
			terms.push_back(Term(coefficient, variable_index));
		}
		void sub_term(uint var_index, double pos_coefficient = 1);
		{
			add_term(-pos_coefficient, variable_index);
		}

		void add_var(uint var_index)
		{
			add_term(1, variable_index);
		}
		void sub_var(uint var_index)
		{
			sub_term(1, variable_index);
		}
}

typedef struct
{
	uint var_index;
	bool has_upper;//upper bound
	bool has_lower;//lower bound
	double upper_bound, lower_bound;
}VariableRange;

typedef pair<LinearExpression *, double> Constraint;
typedef vector<Constraint> Constraints;
typedef vector<VariableRange> VariableRanges;

class LinearProgram
{
		// the function to be maximized
		LinearExpression *objective;

		// linear expressions constrained to an exact value
		Constraints equalities;

		// linear expressions constrained by an upper bound (exp <= bound)
		Constraints inequalities;

		// set of integer variables
		std::set<uint> variables_integer;

		// set of binary variables
		std::set<uint> variables_binary;

		// By default all variables have a lower bound of zero and an upper bound of one.
		// Exceptional cases are stored in this (unsorted) vector.
		VariableRanges non_default_bounds;

	public:
		LinearProgram() : objective(new LinearExpression()) {};

		~LinearProgram()
		{
			delete objective;
			foreach(equalities, eq)
				delete eq->first;
			foreach(inequalities, ineq)
				delete ineq->first;
		}

		void declare_variable_integer(uint variable_index)
		{
			variables_integer.insert(variable_index);
		}

		void declare_variable_binary(uint variable_index)
		{
			variables_binary.insert(variable_index);
		}

		void declare_variable_bounds(uint variable_id,
			bool has_lower, double lower, bool has_upper, double upper)
		{
			VariableRange b;
			b.variable_id = variable_id;
			b.has_lower = has_lower;
			b.has_upper = has_upper;
			b.lower_bound = lower;
			b.upper_bound = upper;
			non_default_bounds.push_back(b);
		}

		void set_objective(LinearExpression *obj)
		{
			delete objective;
			objective = obj;
		}

		void add_inequality(LinearExpression *exp, double upper_bound)
		{
			if (exp->has_terms())
				inequalities.push_back(Constraint(exp, upper_bound));
			else
				delete exp;
		}

		void add_equality(LinearExpression *exp, double equal_to)
		{
			if (exp->has_terms())
				equalities.push_back(Constraint(exp, equal_to));
			else
				delete exp;
		}

		const LinearExpression *get_objective() const
		{
			return objective;
		}

		const set<uint>& get_integer_variables() const
		{
			return variables_integer;
		}

		bool has_binary_variables() const
		{
			return !variables_binary.empty();
		}

		bool has_integer_variables() const
		{
			return !variables_integer.empty();
		}

		bool is_integer_variable(unsigned int variable_id) const
		{
			return variables_integer.find(variable_id) != variables_integer.end();
		}

		bool is_binary_variable(unsigned int variable_id) const
		{
			return variables_binary.find(variable_id) != variables_binary.end();
		}

		const set<uint>& get_binary_variables() const
		{
			return variables_binary;
		}

		const Constraints& get_equalities() const
		{
			return equalities;
		}

		const Constraints& get_inequalities() const
		{
			return inequalities;
		}

		const VariableRanges& get_non_default_variable_ranges() const
		{
			return non_default_bounds;
		}	
}

#endif
