//
//  expr.cpp
//  
//
//  Created by Yuhui on 1/22/20.
//  Copyright © 2020 Yuhui. All rights reserved.
//

#include "expr.hpp"
#include "catch.hpp"
#include "value.hpp"
#include <sstream>
#include "Env.hpp"
#include "step.hpp"
#include "cont.hpp"
#include "parse.hpp"

//NumExpr part
NumExpr::NumExpr(int num) {
  this->num = num;
}

bool NumExpr::equals(PTR(Expr) e) {
  PTR(NumExpr) n = CAST(NumExpr)(e);
  if (n==NULL)
    return false;
  else
    return num == n->num;
}

PTR(Val) NumExpr::to_value(PTR(Env) env) {
    return NEW(NumVal)(num);
}

PTR(Expr) NumExpr::subst(std::string var, PTR(Val) new_val) {
    return NEW(NumExpr)(num);
}

bool NumExpr::containsVariables(){
    return false;
}

PTR(Expr) NumExpr::optimize() {
    return THIS;
}

void NumExpr::step_interp() {
    Step::mode = Step::continue_mode;
    Step::val = NEW(NumVal)(num);
    Step::cont = Step::cont; /* no-op */
}

std::string NumExpr::to_string() {
    return std::to_string(num);
}

//AddExpr part
//
//
AddExpr::AddExpr(PTR(Expr) lhs, PTR(Expr) rhs) {
  this->lhs = lhs;
  this->rhs = rhs;
}

bool AddExpr::equals(PTR(Expr) e) {
  PTR(AddExpr) a = CAST(AddExpr)(e);
  if (a==NULL)
    return false;
  else
    return lhs->equals(a->lhs) && rhs->equals(a->rhs);
}

PTR(Val) AddExpr::to_value(PTR(Env) env) {
    return lhs->to_value(env)->add_to(rhs->to_value(env));
}

PTR(Expr) AddExpr::subst(std::string var, PTR(Val) new_val) {
    return NEW(AddExpr)(lhs->subst(var, new_val), rhs->subst(var, new_val));
}

bool AddExpr::containsVariables(){
    return lhs->containsVariables() || rhs->containsVariables();
}

PTR(Expr) AddExpr::optimize() {
    PTR(Expr) lhs_optimized = lhs->optimize();
    PTR(Expr) rhs_optimized = rhs->optimize();
    
    if (!lhs_optimized->containsVariables() && !rhs_optimized->containsVariables()) {
        PTR(Env) empty_env = NEW(EmptyEnv)();
        PTR(Val) new_val = lhs_optimized->to_value(empty_env)->add_to(rhs_optimized->to_value(empty_env));
        return new_val->to_expr();
    }
    return NEW(AddExpr)(lhs_optimized, rhs_optimized);
}

void AddExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = lhs;
    Step::cont = NEW(RightThenAddCont)(rhs, Step::env, Step::cont);
}

std::string AddExpr::to_string() {
    return "(" + lhs->to_string() + " + " + rhs->to_string() + ")";
}

//MultExpr part
//
//
//
MultExpr::MultExpr(PTR(Expr) lhs, PTR(Expr) rhs) {
  this->lhs = lhs;
  this->rhs = rhs;
}

bool MultExpr::equals(PTR(Expr) e) {
  PTR(MultExpr) m = CAST(MultExpr)(e);
  if (m==NULL)
    return false;
  else
    return (lhs->equals(m->lhs) && rhs->equals(m->rhs));
}

PTR(Val) MultExpr::to_value(PTR(Env) env) {
    return lhs->to_value(env)->mult_with(rhs->to_value(env));
}

PTR(Expr) MultExpr::subst(std::string var, PTR(Val) new_val){
    return NEW(MultExpr)(lhs->subst(var, new_val), rhs->subst(var, new_val));
}

bool MultExpr::containsVariables(){
    return lhs->containsVariables() || rhs->containsVariables();
}

PTR(Expr) MultExpr::optimize() {
    PTR(Expr) lhs_optimized = this->lhs->optimize();
    PTR(Expr) rhs_optimized = this->rhs->optimize();
    
    if (!lhs_optimized->containsVariables() && !rhs_optimized->containsVariables()) {
        PTR(Env) empty_env = NEW(EmptyEnv)();
        PTR(Val) new_val = lhs_optimized->to_value(empty_env)->mult_with(rhs_optimized->to_value(empty_env));
        return new_val->to_expr();
    }
    return NEW(MultExpr)(lhs_optimized, rhs_optimized);
}

void MultExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = lhs;
    Step::cont = NEW(RightThenMultCont)(rhs, Step::env, Step::cont);
}

std::string MultExpr::to_string() {
    return "(" + lhs->to_string() + " * " + rhs->to_string() + ")";
}



// VarExpr part
//
//
//
VarExpr::VarExpr(std::string name) {
  this->name = name;
}

bool VarExpr::equals(PTR(Expr) e) {
  PTR(VarExpr) v = CAST(VarExpr)(e);
  if (v==NULL)
    return false;
  else
    return name == v->name;
}

PTR(Val) VarExpr::to_value(PTR(Env) env) {
    return env->lookup(name);
}

PTR(Expr) VarExpr::subst(std::string var, PTR(Val) new_val) {
    if (name == var)
        return new_val->to_expr();
    else
        return NEW(VarExpr)(name); // or `return THIS`
}

bool VarExpr::containsVariables(){
    return true;
}

PTR(Expr) VarExpr::optimize() {
    return NEW(VarExpr)(name);
}

void VarExpr::step_interp() {
    Step::mode = Step::continue_mode;
    Step::val = Step::env->lookup(name);
    Step::cont = Step::cont; /* no-op */
}

std::string VarExpr::to_string() {
    return name;
}


//BoolExpr part
//
//
BoolExpr::BoolExpr(bool rep) {
  this->rep = rep;
}

bool BoolExpr::equals(PTR(Expr) e) {
  PTR(BoolExpr) b = CAST(BoolExpr)(e);
  if (b==NULL)
    return false;
  else
    return rep == b->rep;
}

PTR(Val) BoolExpr::to_value(PTR(Env) env) {
    return NEW(BoolVal)(rep);
}

PTR(Expr) BoolExpr::subst(std::string var, PTR(Val) new_val) {
    return NEW(BoolExpr)(rep);
}

bool BoolExpr::containsVariables(){
    return false;
}

PTR(Expr) BoolExpr::optimize() {
    return NEW(BoolExpr)(rep);
}

void BoolExpr::step_interp() {
    Step::mode = Step::continue_mode;
    Step::val = NEW(BoolVal)(rep);
    Step::cont = Step::cont; /* no-op */
}

std::string BoolExpr::to_string() {
    if (rep == true)
        return "_true";
    else
        return "_false";
}


// LetExpr part
//
//
LetExpr::LetExpr(std::string name, PTR(Expr) rhs, PTR(Expr) expr){
    this->name = name;
    this->rhs=rhs;
    this->expr=expr;
}

bool LetExpr::equals(PTR(Expr) e){
    PTR(LetExpr) l = CAST(LetExpr)(e);
    
    if(l==NULL)
        return false;
    else
        return name == l->name && rhs->equals(l->rhs) && expr->equals(l->expr);
}

PTR(Val) LetExpr::to_value(PTR(Env) env) {
    PTR(Val) rhs_value = rhs->to_value(env);
    PTR(Env) new_env = NEW(ExtendedEnv)(name, rhs_value, env);
    
    return expr->to_value(new_env);
}

PTR(Expr) LetExpr::subst(std::string var, PTR(Val) val) {
    if (var == name) {
        return NEW(LetExpr)(var, rhs, expr);
    }
    return NEW(LetExpr)(name, rhs->subst(var, val), expr->subst(var, val));
}

bool LetExpr::containsVariables(){
        PTR(Env) empty_env = NEW(EmptyEnv)();
        return rhs->containsVariables() || expr->subst(name, rhs->to_value(empty_env))->containsVariables();
    }

PTR(Expr) LetExpr::optimize() {
    PTR(Expr) rhs_optimized = rhs->optimize();
    PTR(Expr) expr_optimized = expr->optimize();
    
    if (!rhs_optimized->containsVariables()) {
        PTR(Env) empty_env = NEW(EmptyEnv)();
        return expr_optimized->subst(name, rhs_optimized->to_value(empty_env))->optimize();
    }
    return NEW(LetExpr)(name, rhs_optimized, expr_optimized);
}

void LetExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = rhs;
    Step::cont = NEW(LetBodyCont)(name, expr, Step::env, Step::cont);
}

std::string LetExpr::to_string() {
    return "(_let " + name + " = " + rhs->to_string() + " _in " + expr->to_string() + ")";
}


//
//EqualExpr part
//
EqualExpr::EqualExpr(PTR(Expr) lhs, PTR(Expr) rhs){
    this->lhs=lhs;
    this->rhs=rhs;
}

bool EqualExpr::equals(PTR(Expr) e){
    PTR(EqualExpr) a = CAST(EqualExpr)(e);
    if( a == NULL)
        return false;
    return this->lhs->equals(a->lhs) && this->rhs->equals(a->rhs);
}

PTR(Val) EqualExpr::to_value(PTR(Env) env) {
    PTR(Val) lhs_value = lhs->to_value(env);
    PTR(Val) rhs_value = rhs->to_value(env);
    
    if (lhs_value->equals(rhs_value))
        return NEW(BoolVal)(true);
    else
        return NEW(BoolVal)(false);
}

PTR(Expr) EqualExpr::subst(std::string var, PTR(Val) val) {
    return NEW(EqualExpr)(lhs->subst(var, val), rhs->subst(var, val));
}

bool EqualExpr::containsVariables(){
    return lhs->containsVariables() || rhs->containsVariables();
}

PTR(Expr) EqualExpr::optimize() {
    PTR(Expr) lhs_optimized = lhs->optimize();
    PTR(Expr) rhs_optimized = rhs->optimize();
    
    if (!lhs_optimized->containsVariables() && !rhs_optimized->containsVariables())
        return NEW(BoolExpr)(lhs_optimized->equals(rhs_optimized));
    else
        return NEW(EqualExpr)(lhs_optimized, rhs_optimized);
}

void EqualExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = lhs;
    Step::cont = NEW(RightThenCompCont)(rhs, Step::env, Step::cont);
}

std::string EqualExpr::to_string() {
    return "(" + lhs->to_string() + " == " + rhs->to_string() + ")";
}



//
// IfExpr part
//
IfExpr::IfExpr(PTR(Expr) if_part, PTR(Expr) then_part, PTR(Expr) else_part){
    this->if_part = if_part;
    this->then_part = then_part;
    this->else_part = else_part;
}

bool IfExpr::equals(PTR(Expr) e){
    PTR(IfExpr) i = CAST(IfExpr)(e);
    if(i == NULL)
        return false;
    else
        return if_part->equals(i->if_part) && then_part->equals(i->then_part) && else_part->equals(i->else_part);
}

PTR(Val) IfExpr::to_value(PTR(Env) env) {
    PTR(Val) if_value= if_part->to_value(env);
    
    if (if_value->equals(NEW(BoolVal)(true))) {
        return then_part->to_value(env);
    } else {
        return else_part->to_value(env);
    }
}

PTR(Expr) IfExpr::subst(std::string var, PTR(Val) val) {
    return NEW(IfExpr)(if_part->subst(var, val), then_part->subst(var, val), else_part->subst(var, val));
}

bool IfExpr::containsVariables() {
    if (if_part->containsVariables()) {
        return true;
    }
    PTR(Env) empty_env = NEW(EmptyEnv)();
    PTR(Val) if_value = if_part->to_value(empty_env);
    
    if (if_value->equals(NEW(BoolVal)(true)))
        return then_part->containsVariables();
    else
        return else_part->containsVariables();
}

PTR(Expr) IfExpr::optimize() {
    PTR(Expr) if_part_optimized = if_part->optimize();
    
    if (if_part_optimized->equals(NEW(BoolExpr)(true))) {
        return then_part->optimize();
    } else if (if_part_optimized->equals(NEW(BoolExpr)(false))){
        return else_part->optimize();
    } else {
        return NEW(IfExpr)(if_part_optimized, then_part->optimize(), else_part->optimize());
    }
}

void IfExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = if_part;
    Step::cont = NEW(IfBranchCont)(then_part, else_part, Step::env, Step::cont);
}

std::string IfExpr::to_string() {
    return "(_if " + if_part->to_string()
    + " _then " + then_part->to_string()
    + " _else " + else_part->to_string() + ")";
}


//funExpr part
//
//
FunExpr::FunExpr(std::string formal_arg, PTR(Expr) body) {
    this->formal_arg = formal_arg;
    this->body = body;
}

bool FunExpr::equals(PTR(Expr) e) {
    PTR(FunExpr) f = CAST(FunExpr)(e);
    
    if (f == NULL)
        return false;
    else
        return formal_arg == f->formal_arg && body->equals(f->body);
}

PTR(Val) FunExpr::to_value(PTR(Env) env) {
    return NEW(FunVal)(formal_arg, body, env);
}

PTR(Expr) FunExpr::subst(std::string var, PTR(Val) val) {
    if (var == formal_arg)
        return NEW(FunExpr)(formal_arg, body);
    else
        return NEW(FunExpr)(formal_arg, body->subst(var, val));
}

bool FunExpr::containsVariables() {
        return body->subst(formal_arg, NEW(NumVal)(0))->containsVariables();
}

PTR(Expr) FunExpr::optimize() {
    return NEW(FunExpr)(formal_arg, body->optimize());
}

void FunExpr::step_interp() {
    Step::mode = Step::continue_mode;
    Step::val = NEW(FunVal)(formal_arg, body, Step::env);
}

std::string FunExpr::to_string() {
    return "(_fun(" + formal_arg + ") " + body->to_string() + ")";
}


//callExpr part
//
//
CallFunExpr::CallFunExpr(PTR(Expr) to_be_called, PTR(Expr) actual_arg) {
    this->to_be_called = to_be_called;
    this->actual_arg = actual_arg;
}

bool CallFunExpr::equals(PTR(Expr) e) {
    PTR(CallFunExpr) c = CAST(CallFunExpr)(e);
    if (c == NULL)
        return false;
    else
        return to_be_called->equals(c->to_be_called) && actual_arg->equals(c->actual_arg);
}

PTR(Val) CallFunExpr::to_value(PTR(Env) env) {
    return to_be_called->to_value(env)->call(actual_arg->to_value(env));
}

PTR(Expr) CallFunExpr::subst(std::string var, PTR(Val) val) {
    return NEW(CallFunExpr)(to_be_called->subst(var, val), actual_arg->subst(var, val));
}

bool CallFunExpr::containsVariables() {
    return actual_arg->containsVariables() || to_be_called->containsVariables();
}

PTR(Expr) CallFunExpr::optimize() {
    return NEW(CallFunExpr)(to_be_called->optimize(), actual_arg->optimize());
}

void CallFunExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = to_be_called;
    Step::cont = NEW(ArgThenCallCont)(actual_arg, Step::env, Step::cont);
}

std::string CallFunExpr::to_string() {
    return to_be_called->to_string() + "(" + actual_arg->to_string() + ")";
}

static std::string evaluate_expr(PTR(Expr) expr) {
    try {
        PTR(EmptyEnv) empty_env = NEW(EmptyEnv)();
        (void)expr->to_value(empty_env);
        return "";
    } catch (std::runtime_error exn) {
        return exn.what();
    }
}


TEST_CASE( "equals" ) {
    // equals method for NumExpr
    CHECK( (NEW(NumExpr)(1))->equals(NEW(NumExpr)(1)) );
    CHECK( ! (NEW(NumExpr)(0))->equals(NEW(NumExpr)(2)) );
    CHECK( ! (NEW(NumExpr)(2))->equals(NEW(AddExpr)(NEW(NumExpr)(1), NEW(NumExpr)(1))) );
    CHECK( ! (NEW(NumExpr)(2))->equals(NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(1))) );
    CHECK( ! (NEW(NumExpr)(1))->equals(NEW(VarExpr)("x")) );
    CHECK( ! (NEW(NumExpr)(1))->equals(NEW(BoolExpr)(true)) );
    
    // equals method for AddExpr
    CHECK( (NEW(AddExpr)(NEW(NumExpr)(1), NEW(NumExpr)(2)))
          ->equals(NEW(AddExpr)(NEW(NumExpr)(1), NEW(NumExpr)(2))) );
    CHECK( ! (NEW(AddExpr)(NEW(NumExpr)(1), NEW(NumExpr)(9)))
          ->equals(NEW(AddExpr)(NEW(NumExpr)(1), NEW(NumExpr)(10))) );
    CHECK( ! (NEW(AddExpr)(NEW(NumExpr)(1), NEW(NumExpr)(9)))
          ->equals(NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(9))) );
    CHECK( ! (NEW(AddExpr)(NEW(NumExpr)(1), NEW(NumExpr)(9)))
          ->equals(NEW(NumExpr)(1)) );
    CHECK( ! (NEW(AddExpr)(NEW(NumExpr)(1), NEW(NumExpr)(9)))
          ->equals(NEW(AddExpr)(NEW(NumExpr)(9), NEW(NumExpr)(1))) );
    
    // equals method for MultExpr
    CHECK( (NEW(MultExpr)(NEW(NumExpr)(1), NEW(NumExpr)(2)))
          ->equals(NEW(MultExpr)(NEW(NumExpr)(1), NEW(NumExpr)(2))) );
    CHECK( ! (NEW(MultExpr)(NEW(NumExpr)(1), NEW(NumExpr)(2)))
          ->equals(NEW(MultExpr)(NEW(NumExpr)(1), NEW(NumExpr)(10))) );
    CHECK( ! (NEW(MultExpr)(NEW(NumExpr)(1), NEW(NumExpr)(2)))
          ->equals(NEW(MultExpr)(NEW(NumExpr)(10), NEW(NumExpr)(7))) );
    CHECK( ! (NEW(MultExpr)(NEW(NumExpr)(1), NEW(NumExpr)(2)))
          ->equals(NEW(NumExpr)(1)) );
    CHECK( ! (NEW(MultExpr)(NEW(NumExpr)(1), NEW(NumExpr)(2)))
          ->equals(NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(1))) );
    
    // equals method for VarExpr
    CHECK( (NEW(VarExpr)("x"))->equals(NEW(VarExpr)("x")) );
    CHECK( !(NEW(VarExpr)("x"))->equals(NEW(VarExpr)("y")) );
    CHECK( ! (NEW(VarExpr)("xyz"))->equals(NEW(VarExpr)("zyx")) );
    CHECK( ! (NEW(VarExpr)("z"))->equals(NEW(NumExpr)(5)) );
    
    // equals method for BoolExpr
    CHECK( (NEW(BoolExpr)(true))->equals(NEW(BoolExpr)(true)) );
    CHECK( !(NEW(BoolExpr)(true))->equals(NEW(BoolExpr)(false)) );
    CHECK( (NEW(BoolExpr)(false))->equals(NEW(BoolExpr)(false)) );
    CHECK( !(NEW(BoolExpr)(false))->equals(NEW(VarExpr)("false")) );
    CHECK( !(NEW(BoolExpr)(true))->equals(NEW(NumExpr)(1)) );
    CHECK( !(NEW(BoolExpr)(false))->equals(NEW(NumExpr)(0)) );
    
    // equals method for LetExpr
    CHECK( !(NEW(LetExpr)("x", NEW(NumExpr)(1), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4))))
          ->equals(NEW(NumExpr)(5)) );
    CHECK( (NEW(LetExpr)("x", NEW(NumExpr)(1), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4))))
          ->equals(NEW(LetExpr)("x", NEW(NumExpr)(1), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4)))) );
    
    // equals method for IfExpr
    CHECK( !(NEW(IfExpr)(NEW(BoolExpr)(true), NEW(NumExpr)(3), NEW(NumExpr)(4)))
          ->equals(NEW(IfExpr)(NEW(BoolExpr)(true), NEW(NumExpr)(3), NEW(NumExpr)(5))) );
    
    CHECK( (NEW(IfExpr)(NEW(EqualExpr)(NEW(NumExpr)(3), NEW(NumExpr)(3)), NEW(BoolExpr)(true), NEW(BoolExpr)(false)))
          ->equals((NEW(IfExpr)(NEW(EqualExpr)(NEW(NumExpr)(3), NEW(NumExpr)(3)), NEW(BoolExpr)(true), NEW(BoolExpr)(false)))) );
    
    CHECK( !(NEW(IfExpr)(NEW(EqualExpr)(NEW(NumExpr)(3), NEW(NumExpr)(3)), NEW(BoolExpr)(true), NEW(BoolExpr)(false)))
          ->equals((NEW(IfExpr)(NEW(BoolExpr)(true), NEW(BoolExpr)(true), NEW(BoolExpr)(false)))) );
    
    // equals method for EqualExpr
    CHECK( !(NEW(EqualExpr)(NEW(NumExpr)(3), NEW(NumExpr)(1)))
          ->equals(NEW(EqualExpr)(NEW(NumExpr)(3), NEW(BoolExpr)(true))) );
    CHECK( (NEW(EqualExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4)))
          ->equals(NEW(EqualExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4))) );
    
    // equals method for FunExpr
    CHECK( (NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))))
          ->equals(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x")))) );
    CHECK( !(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))))
          ->equals(NEW(FunExpr)("x", NEW(MultExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x")))) );
    
    // equals method for CallFunExpr
    CHECK( (NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))), NEW(NumExpr)(4)))
          ->equals(NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))), NEW(NumExpr)(4))) );
    CHECK( !(NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))), NEW(NumExpr)(4)))
          ->equals(NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(MultExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))), NEW(NumExpr)(4))) );
}

TEST_CASE( "to_value") {
    // to_value method for NumExpr
    CHECK( (NEW(NumExpr)(10))->to_value(Env::emptyenv)->equals(NEW(NumVal)(10)) );
    CHECK( !(NEW(NumExpr)(10))->to_value(Env::emptyenv)->equals(NEW(NumVal)(100)) );
    
    CHECK( Step::interp_by_steps(NEW(NumExpr)(10))->equals(NEW(NumVal)(10)) );
    CHECK( !Step::interp_by_steps(NEW(NumExpr)(10))->equals(NEW(NumVal)(100)) );
    
    // to_value method for AddExpr
    CHECK( (NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(2)))->to_value(Env::emptyenv)
          ->equals(NEW(NumVal)(5)) );
    CHECK( !(NEW(AddExpr)(NEW(NumExpr)(0), NEW(NumExpr)(1)))->to_value(Env::emptyenv)
          ->equals(NEW(BoolVal)(true)) );
    
    CHECK( Step::interp_by_steps(NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(2)))
          ->equals(NEW(NumVal)(5)) );
    CHECK( !Step::interp_by_steps(NEW(AddExpr)(NEW(NumExpr)(0), NEW(NumExpr)(1)))
          ->equals(NEW(BoolVal)(true)) );
    
    // to_value method for MultExpr
    CHECK( (NEW(MultExpr)(NEW(NumExpr)(3), NEW(NumExpr)(2)))->to_value(Env::emptyenv)
          ->equals(NEW(NumVal)(6)) );
    CHECK( !(NEW(MultExpr)(NEW(NumExpr)(0), NEW(NumExpr)(1)))->to_value(Env::emptyenv)
          ->equals(NEW(BoolVal)(false)) );
    
    CHECK( Step::interp_by_steps(NEW(MultExpr)(NEW(NumExpr)(3), NEW(NumExpr)(2)))
          ->equals(NEW(NumVal)(6)) );
    CHECK( !Step::interp_by_steps(NEW(MultExpr)(NEW(NumExpr)(0), NEW(NumExpr)(1)))
          ->equals(NEW(BoolVal)(false)) );
    
    // to_value method for VarExpr
    CHECK( evaluate_expr(NEW(VarExpr)("fish")) == "free variable: fish" );
    CHECK( evaluate_expr(NEW(VarExpr)("me")) == "free variable: me" );
    
    // to_value method for BoolExpr
    CHECK( (NEW(BoolExpr)(true))->to_value(Env::emptyenv)->equals(NEW(BoolVal)(true)) );
    CHECK( (NEW(BoolExpr)(false))->to_value(Env::emptyenv)->equals(NEW(BoolVal)(false)) );
    CHECK( !(NEW(BoolExpr)(false))->to_value(Env::emptyenv)->equals(NEW(BoolVal)(true)) );
    
    CHECK( Step::interp_by_steps(NEW(BoolExpr)(true))->equals(NEW(BoolVal)(true)) );
    CHECK( Step::interp_by_steps(NEW(BoolExpr)(false))->equals(NEW(BoolVal)(false)) );
    CHECK( !Step::interp_by_steps(NEW(BoolExpr)(false))->equals(NEW(BoolVal)(true)) );
    
    // to_value method for LetExpr
    CHECK( (NEW(LetExpr)("x", NEW(NumExpr)(1), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4))))
          ->to_value(Env::emptyenv)->equals(NEW(NumVal)(5)));
    CHECK( evaluate_expr(NEW(LetExpr)("x", NEW(VarExpr)("y"), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4))))
          == "free variable: y" );
    CHECK( (NEW(LetExpr)("x", NEW(NumExpr)(1), NEW(LetExpr)("x", NEW(NumExpr)(2), NEW(VarExpr)("x"))))
          ->to_value(Env::emptyenv)->equals(NEW(NumVal)(2)) );
    
    CHECK( Step::interp_by_steps(NEW(LetExpr)("x", NEW(NumExpr)(1), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4))))
          ->equals(NEW(NumVal)(5)));
    CHECK( Step::interp_by_steps(NEW(LetExpr)("x", NEW(NumExpr)(1), NEW(LetExpr)("x", NEW(NumExpr)(2), NEW(VarExpr)("x"))))
          ->equals(NEW(NumVal)(2)) );
    
    // to_value for IfExpr
    CHECK( (NEW(IfExpr)(NEW(BoolExpr)(false), NEW(NumExpr)(3), NEW(NumExpr)(6)))
          ->to_value(Env::emptyenv)->equals(NEW(NumVal)(6)) );
    CHECK( evaluate_expr(NEW(IfExpr)(NEW(EqualExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4)), NEW(BoolExpr)(true), NEW(BoolExpr)(false)))
          == "free variable: x" );
    CHECK( !(NEW(IfExpr)(NEW(EqualExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4)), NEW(NumExpr)(3), NEW(NumExpr)(6)))
          ->to_value(Env::emptyenv)->equals(NEW(NumVal)(3)) );
    
    CHECK( Step::interp_by_steps(NEW(IfExpr)(NEW(BoolExpr)(false), NEW(NumExpr)(3), NEW(NumExpr)(6)))
          ->equals(NEW(NumVal)(6)) );
    CHECK( !Step::interp_by_steps(NEW(IfExpr)(NEW(EqualExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4)), NEW(NumExpr)(3), NEW(NumExpr)(6)))
          ->equals(NEW(NumVal)(3)) );
    
    // to_value for EqualExpr
    CHECK((NEW(EqualExpr)(NEW(BoolExpr)(true), NEW(BoolExpr)(true)))
          ->to_value(Env::emptyenv)->equals((NEW(BoolVal)(true))) );
    CHECK((NEW(EqualExpr)(NEW(AddExpr)(NEW(NumExpr)(6), NEW(NumExpr)(6)), NEW(MultExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4))))
          ->to_value(Env::emptyenv)->equals((NEW(BoolVal)(true))) );
    CHECK(evaluate_expr(NEW(EqualExpr)(NEW(VarExpr)("xyz"), NEW(NumExpr)(2)))
          == "free variable: xyz" );
    
    CHECK( Step::interp_by_steps(NEW(EqualExpr)(NEW(BoolExpr)(true), NEW(BoolExpr)(true)))
          ->equals((NEW(BoolVal)(true))) );
    CHECK( Step::interp_by_steps(NEW(EqualExpr)(NEW(AddExpr)(NEW(NumExpr)(6), NEW(NumExpr)(6)), NEW(MultExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4))))
          ->equals((NEW(BoolVal)(true))) );
    
    // to_value for FunExpr
    CHECK( (NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))))->to_value(Env::emptyenv)
          ->to_string() == "[FUNCTION]" );
    CHECK( !((NEW(FunExpr)("y", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))))->to_value(Env::emptyenv)
             ->to_string() == "FUNCTION]") );
    
    CHECK( Step::interp_by_steps(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))))
          ->to_string() == "[FUNCTION]" );
    CHECK( !(Step::interp_by_steps(NEW(FunExpr)("y", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))))
             ->to_string() == "FUNCTION]") );
    
    // to_value for CallFunExpr
    CHECK( (NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))), NEW(NumExpr)(4)))
          ->to_value(Env::emptyenv)->equals(NEW(NumVal)(8)));
    CHECK( !(NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(MultExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))), NEW(NumExpr)(4)))
          ->to_value(Env::emptyenv)->equals(NEW(NumVal)(15)));
    
    CHECK( Step::interp_by_steps(NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))), NEW(NumExpr)(4)))
          ->equals(NEW(NumVal)(8)));
    CHECK( !Step::interp_by_steps(NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(MultExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))), NEW(NumExpr)(4)))
          ->equals(NEW(NumVal)(15)));
}


TEST_CASE( "subst") {
    // subst method for NumExpr
    CHECK( (NEW(NumExpr)(10))->subst("dog", NEW(NumVal)(3))
          ->equals(NEW(NumExpr)(10)) );
    CHECK( !(NEW(NumExpr)(10))->subst("dog", NEW(NumVal)(9))
          ->equals(NEW(NumExpr)(9)) );
    
    // subst method for AddExpr
    CHECK( (NEW(AddExpr)(NEW(NumExpr)(2), NEW(VarExpr)("dog")))->subst("dog", NEW(NumVal)(3))
          ->equals(NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3))) );
    CHECK( !(NEW(AddExpr)(NEW(VarExpr)("cat"), NEW(VarExpr)("dog")))->subst("dog", NEW(NumVal)(3))
          ->equals(NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3))) );
    
    // subst method for MultExpr
    CHECK( (NEW(MultExpr)(NEW(NumExpr)(2), NEW(VarExpr)("dog")))->subst("dog", NEW(NumVal)(3))
          ->equals(NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3))) );
    CHECK( !(NEW(MultExpr)(NEW(VarExpr)("cat"), NEW(VarExpr)("dog")))->subst("dog", NEW(NumVal)(3))
          ->equals(NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3))) );
    
    // subst method for VarExpr
    CHECK( (NEW(VarExpr)("fish"))->subst("dog", NEW(NumVal)(3))
          ->equals(NEW(VarExpr)("fish")) );
    CHECK( (NEW(VarExpr)("dog"))->subst("dog", NEW(NumVal)(3) )
          ->equals(NEW(NumExpr)(3)) );
    CHECK( (NEW(VarExpr)("dog"))->subst("dog", NEW(BoolVal)(true) )
          ->equals(NEW(BoolExpr)(true)) );
    CHECK( !(NEW(VarExpr)("cat"))->subst("dog", NEW(NumVal)(3) )
          ->equals(NEW(NumExpr)(3)) );
    
    // subst method for BoolExpr
    CHECK( (NEW(BoolExpr)(true))->subst("x", NEW(NumVal)(3))->equals(NEW(BoolExpr)(true)) );
    CHECK( !(NEW(BoolExpr)(false))->subst("false", NEW(NumVal)(3))->equals(NEW(NumExpr)(3)) );
    
    // subst method for LetExpr
    CHECK( (NEW(LetExpr)("x", NEW(VarExpr)("y"), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4))))
          ->subst("y", NEW(NumVal)(3))
          ->equals(NEW(LetExpr)("x", NEW(NumExpr)(3), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4)))) );
    CHECK( !(NEW(LetExpr)("x", NEW(VarExpr)("z"), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4))))
          ->subst("y", NEW(NumVal)(3))
          ->equals(NEW(LetExpr)("x", NEW(NumExpr)(3), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4)))) );
    
    CHECK( (NEW(LetExpr)("x", NEW(NumExpr)(3), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4))))
          ->subst("x", NEW(NumVal)(7))
          ->equals(NEW(LetExpr)("x", NEW(NumExpr)(3), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4)))) );
    
    // subst method for IfExpr
    CHECK( (NEW(IfExpr)(NEW(EqualExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3)), NEW(BoolExpr)(true), NEW(BoolExpr)(false)))
          ->subst("x", NEW(NumVal)(3))
          ->equals(NEW(IfExpr)(NEW(EqualExpr)(NEW(NumExpr)(3), NEW(NumExpr)(3)), NEW(BoolExpr)(true), NEW(BoolExpr)(false))) );
    
    CHECK( !(NEW(IfExpr)(NEW(EqualExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3)), NEW(BoolExpr)(true), NEW(BoolExpr)(false)))
          ->subst("y", NEW(NumVal)(3))
          ->equals(NEW(IfExpr)(NEW(EqualExpr)(NEW(NumExpr)(3), NEW(NumExpr)(3)), NEW(BoolExpr)(true), NEW(BoolExpr)(false))) );
    
    // subst method for EqualExpr
    CHECK( (NEW(EqualExpr)(NEW(VarExpr)("x"), NEW(BoolExpr)(true)))
          ->subst("x", NEW(NumVal)(124))
          ->equals(NEW(EqualExpr)(NEW(NumExpr)(124), NEW(BoolExpr)(true))) );
    
    CHECK( !(NEW(EqualExpr)(NEW(VarExpr)("x"), NEW(BoolExpr)(true)))
          ->subst("yx", NEW(NumVal)(124))
          ->equals(NEW(EqualExpr)(NEW(NumExpr)(124), NEW(BoolExpr)(true))) );
    
    CHECK( (NEW(EqualExpr)(NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4)), NEW(MultExpr)(NEW(NumExpr)(12), NEW(VarExpr)("x"))))
          ->subst("x", NEW(NumVal)(124))
          ->equals(NEW(EqualExpr)(NEW(AddExpr)(NEW(NumExpr)(124), NEW(NumExpr)(4)), NEW(MultExpr)(NEW(NumExpr)(12), NEW(NumExpr)(124)))) );
    
    // subst method for FunExpr
    CHECK( (NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))))->subst("x", NEW(NumVal)(3))
          ->equals( NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x")))) );
    CHECK( (NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("y"))))->subst("y", NEW(NumVal)(3))
          ->equals( NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3)))) );
    CHECK( !(NEW(FunExpr)("x", NEW(MultExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("y"))))->subst("x", NEW(NumVal)(3))
          ->equals( NEW(FunExpr)("x", NEW(AddExpr)(NEW(NumExpr)(3), NEW(VarExpr)("y")))) );
    
    // subst method for CallFunExpr
    CHECK( (NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))), NEW(NumExpr)(4)))
          ->subst("x", NEW(NumVal)(3))
          ->equals((NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))), NEW(NumExpr)(4)))) );
    CHECK( (NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("y"))), NEW(NumExpr)(4)))
          ->subst("y", NEW(NumVal)(3))
          ->equals((NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3))), NEW(NumExpr)(4)))) );
    CHECK( !(NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(MultExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))), NEW(NumExpr)(4)))
          ->subst("x", NEW(NumVal)(3))
          ->equals((NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(3))), NEW(NumExpr)(4)))) );
}

TEST_CASE("containsVariables") {
    // containsVariables method for NumExpr
    CHECK( !(NEW(NumExpr)(3))->containsVariables() );
    CHECK( !(NEW(NumExpr)(0))->containsVariables() );
    
    // containsVariables method for AddExpr
    CHECK( !(NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)))->containsVariables() );
    CHECK( (NEW(AddExpr)(NEW(NumExpr)(2), NEW(VarExpr)("x")))->containsVariables() );
    
    // containsVariables method for MultExpr
    CHECK( !(NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)))->containsVariables() );
    CHECK( (NEW(MultExpr)(NEW(NumExpr)(2), NEW(VarExpr)("x")))->containsVariables() );
    
    // containsVariables method for VarExpr
    CHECK( (NEW(VarExpr)("x"))->containsVariables() ) ;
    CHECK( (NEW(VarExpr)("xyz"))->containsVariables() );
    
    // containsVariables method for BoolExpr
    CHECK( !(NEW(BoolExpr)(true))->containsVariables()) ;
    CHECK( !(NEW(BoolExpr)(false))->containsVariables() );
    
    // containsVariables method for LetExpr
    CHECK( (NEW(LetExpr)("x", NEW(VarExpr)("y"), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4))))
          ->containsVariables() );
    CHECK( !(NEW(LetExpr)("x", NEW(NumExpr)(3), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4))))
          ->containsVariables() );
    
    // containsVariables method for IfExpr
    CHECK( !(NEW(LetExpr)("x", NEW(NumExpr)(1), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4))))
          ->containsVariables() );
    
    CHECK( (NEW(IfExpr)(NEW(EqualExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3)), NEW(BoolExpr)(true), NEW(BoolExpr)(false)))
          ->containsVariables() );
    
    // containsVariablesmethod for EqualExpr
    CHECK( (NEW(EqualExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3)))->containsVariables() );
    CHECK( !(NEW(EqualExpr)(NEW(BoolExpr)(true), NEW(NumExpr)(3)))->containsVariables() );
    
    // containsVariables method for FunExpr
    CHECK( !(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))))->containsVariables() );
    CHECK( (NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("y"))))->containsVariables() );
    
    // containsVariables method for CallFunExpr
    CHECK( !(NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))), NEW(NumExpr)(4)))->containsVariables() );
    CHECK( (NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("y"))), NEW(NumExpr)(4)))->containsVariables() );
}

TEST_CASE("optimize") {
    // optimize method for NumExpr
    CHECK( (NEW(NumExpr)(3))->optimize()->equals(NEW(NumExpr)(3)) );
    CHECK( !(NEW(NumExpr)(3))->optimize()->equals(NEW(NumExpr)(8)) );
    
    // optimize method for AddExpr
    CHECK( (NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4)))->optimize()->equals(NEW(NumExpr)(7)) );
    CHECK( (NEW(AddExpr)(NEW(NumExpr)(3), NEW(VarExpr)("x")))->optimize()
          ->equals(NEW(AddExpr)(NEW(NumExpr)(3), NEW(VarExpr)("x"))) );
    
    // optimize method for MultExpr
    CHECK( (NEW(MultExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4)))->optimize()->equals(NEW(NumExpr)(12)) );
    CHECK( (NEW(MultExpr)(NEW(NumExpr)(3), NEW(VarExpr)("x")))->optimize()
          ->equals(NEW(MultExpr)(NEW(NumExpr)(3), NEW(VarExpr)("x"))) );
    
    // optimize method for VarExpr
    CHECK( (NEW(VarExpr)("x"))->optimize()->equals(NEW(VarExpr)("x")) );
    CHECK( !(NEW(VarExpr)("x"))->optimize()->equals(NEW(VarExpr)("y")) );
    
    // optimize method for BoolExpr
    CHECK( (NEW(BoolExpr)(true))->optimize()->equals(NEW(BoolExpr)(true)) );
    CHECK( !(NEW(BoolExpr)(true))->optimize()->equals(NEW(BoolExpr)(false)) );
    
    // optimize method for LetExpr
    CHECK( (NEW(LetExpr)("x", NEW(VarExpr)("y"), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4))))
          ->optimize()
          ->equals(NEW(LetExpr)("x", NEW(VarExpr)("y"), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4)))) );
    CHECK( (NEW(LetExpr)("x", NEW(NumExpr)(3), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4))))
          ->optimize()->equals(NEW(NumExpr)(7)) );
    
    // optimize method for IfExpr
    CHECK( (NEW(IfExpr)(NEW(EqualExpr)(NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(9)), NEW(MultExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4))), NEW(BoolExpr)(true), NEW(BoolExpr)(false)))
          ->optimize()->equals(NEW(BoolExpr)(true)));
    
    CHECK( (NEW(IfExpr)(NEW(EqualExpr)(NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(9)), NEW(MultExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4))), NEW(BoolExpr)(true), NEW(BoolExpr)(false)))
          ->optimize()->equals(NEW(IfExpr)(NEW(EqualExpr)(NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(9)), NEW(NumExpr)(12)), NEW(BoolExpr)(true), NEW(BoolExpr)(false))));
    
    // optimize method for EqualExpr
    CHECK( (NEW(EqualExpr)(NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(9)), NEW(MultExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4))))
          ->optimize()->equals(NEW(BoolExpr)(true)));
    
    CHECK( (NEW(EqualExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x")))
          ->optimize()->equals(NEW(EqualExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))));
    
    CHECK( !(NEW(EqualExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x")))
          ->optimize()->equals(NEW(BoolExpr)(true)));
    
    // optimize method for FunExpr
    CHECK( (NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))))->optimize()
          ->equals(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x")))) );
    CHECK( (NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4)))))->optimize()
          ->equals(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(7)))) );
    CHECK( !(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(MultExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4)))))->optimize()
          ->equals(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(7)))) );
    
    
    // optimize method for CallFunExpr
    CHECK( (NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))), NEW(NumExpr)(4)))
          ->optimize()->equals((NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))), NEW(NumExpr)(4)))) );
    
    CHECK( !(NEW(CallFunExpr)(NEW(FunExpr)("x", NEW(MultExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))), NEW(NumExpr)(4)))
          ->optimize()->equals(NEW(NumExpr)(16)) );
    
}
