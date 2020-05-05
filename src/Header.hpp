#include <string>
#include <sstream>


std::string interp(std::istream& in);

std::string optimize(std::istream& in);

std::string step_interp(std::istream &in);

parse(std::istream &in);

virtual bool equals(PTR(Expr) e);

virtual PTR(Val) interp(PTR(Env) env);

virtual void step_interp();

virtual PTR(Expr) optimize();

virtual bool equals(PTR(Val) val);

virtual PTR(Expr) to_expr;

virtual std::string to_string();

virtual PTR(Val) call(PRT(Val) actual_arg);

virtual void call_step(PTR(Val) actual_arg_val, PTR(Cont) rest);

PTR(Val) lookup(std::string find_name);

bool equals(PTR(Env) env);

virtual void step_continue();

static PTR(Val) interp_by_steps(PTR(Expr) e);
