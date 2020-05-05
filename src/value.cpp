//
//  value.cpp
//  
//
//  Created by Yuhui on 1/22/20.
//  Copyright © 2020 Yuhui. All rights reserved.
//

#include "expr.hpp"
#include <stdexcept>
#include "catch.hpp"
#include "Env.hpp"
#include "value.hpp"
#include "step.hpp"
#include "parse.hpp"

/**
 Num part
 */
NumVal::NumVal(int rep) {
  this->rep = rep;
}

bool NumVal::equals(PTR(Val) other_val) {
    PTR(NumVal) other_num_val = CAST(NumVal)(other_val);
    if (other_num_val == nullptr)
        return false;
    else
        return rep == other_num_val->rep;
}

PTR(Val) NumVal::add_to(PTR(Val) other_val) {
    PTR(NumVal) other_num_val = CAST(NumVal)(other_val);
    if (other_num_val == nullptr)
        throw std::runtime_error("input is not a number");
    else
        return NEW(NumVal)(rep + other_num_val->rep);
}

PTR(Val) NumVal::mult_with(PTR(Val) other_val) {
    PTR(NumVal) other_num_val = CAST(NumVal)(other_val);
    if (other_num_val == nullptr)
        throw std::runtime_error("input is not a number");
    else
        return NEW(NumVal)(rep * other_num_val->rep);
}

PTR(Expr) NumVal::to_expr() {
    return NEW(NumExpr)(rep);
}

std::string NumVal::to_string() {
  return std::to_string(rep);
}

PTR(Val) NumVal::call(PTR(Val) actual_arg) {
    throw std::runtime_error("Error");
}

void NumVal::call_step(PTR(Val) actual_arg_val, PTR(Cont) rest) {
    throw std::runtime_error("wrong function call");
}


/**
 Bool part
 */
BoolVal::BoolVal(bool rep) {
  this->rep = rep;
}

bool BoolVal::equals(PTR(Val) other_val) {
    PTR(BoolVal) other_bool_val = CAST(BoolVal)(other_val);
    if (other_bool_val == nullptr)
        return false;
    else
        return rep == other_bool_val->rep;
}

PTR(Val) BoolVal::add_to(PTR(Val) other_val) {
    throw std::runtime_error("cannot add booleans");
}

PTR(Val) BoolVal::mult_with(PTR(Val) other_val) {
    throw std::runtime_error("cannot multiply booleans");
}

PTR(Expr) BoolVal::to_expr() {
    return NEW(BoolExpr)(rep);
}

std::string BoolVal::to_string() {
  if (rep)
    return "_true";
  else
    return "_false";
}

PTR(Val) BoolVal::call(PTR(Val) actual_arg) {
    throw std::runtime_error("error with function call");
}

void BoolVal::call_step(PTR(Val) actual_arg_val, PTR(Cont) rest) {
    throw std::runtime_error("wrong function call");
}



/**
 Fun part
 */
FunVal::FunVal(std::string formal_arg, PTR(Expr) body, PTR(Env) env) {
    this->formal_arg = formal_arg;
    this->body = body;
    this->env = env;
}


bool FunVal::equals(PTR(Val) val) {
    PTR(FunVal) f = CAST(FunVal)(val);
    if (f == NULL)
        return false;
    else
        return formal_arg == f->formal_arg && body->equals(f->body) && env->equals(f->env);
}

PTR(Val) FunVal::add_to(PTR(Val) other_val) {
    throw std::runtime_error("cannot add functions");
}

PTR(Val) FunVal::mult_with(PTR(Val) other_val) {
    throw std::runtime_error("cannot multiply functions");
}

PTR(Expr) FunVal::to_expr() {
    return NEW(FunExpr)(formal_arg, body);
}

std::string FunVal::to_string() {
    return "[FUNCTION]";
}

PTR(Val) FunVal::call(PTR(Val) actual_arg) {
    return body->to_value(NEW(ExtendedEnv)(formal_arg, actual_arg, env));
}

void FunVal::call_step(PTR(Val) actual_arg_val, PTR(Cont) rest) {
    Step::mode = Step::interp_mode;
    Step::expr = body;
    Step::env = NEW(ExtendedEnv)(formal_arg, actual_arg_val, env);
    Step::cont = rest;
}

TEST_CASE( "value equals" ) {
    CHECK( (NEW(NumVal)(5))->equals(NEW(NumVal)(5)) );
    CHECK( ! (NEW(NumVal)(7))->equals(NEW(NumVal)(5)) );
    
    CHECK( (NEW(BoolVal)(true))->equals(NEW(BoolVal)(true)) );
    CHECK( ! (NEW(BoolVal)(true))->equals(NEW(BoolVal)(false)) );
    CHECK( ! (NEW(BoolVal)(false))->equals(NEW(BoolVal)(true)) );
    
    CHECK( ! (NEW(NumVal)(7))->equals(NEW(BoolVal)(false)) );
    CHECK( ! (NEW(BoolVal)(false))->equals(NEW(NumVal)(8)) );
        
    CHECK( (NEW(FunVal)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x")), Env::emptyenv))
          ->equals(NEW(FunVal)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x")), Env::emptyenv)) );
    CHECK( !(NEW(FunVal)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x")), Env::emptyenv))
          ->equals(NEW(FunVal)("x", NEW(MultExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x")), Env::emptyenv)) );
    
}

TEST_CASE( "add_to" ) {
    
    CHECK ( (NEW(NumVal)(5))->add_to(NEW(NumVal)(8))->equals(NEW(NumVal)(13)) );
    
    CHECK_THROWS_WITH ( (NEW(NumVal)(5))->add_to(NEW(BoolVal)(false)), "input is not a number" );
    CHECK_THROWS_WITH ( (NEW(BoolVal)(false))->add_to(NEW(BoolVal)(false)), "cannot add booleans" );
    
    CHECK_THROWS_WITH( (NEW(FunVal)("x", NEW(MultExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x")), Env::emptyenv))->add_to(NEW(NumVal)(4)), "cannot add functions");
}

TEST_CASE( "mult_with" ) {
    
    CHECK ( (NEW(NumVal)(5))->mult_with(NEW(NumVal)(8))->equals(NEW(NumVal)(40)) );
    
    CHECK_THROWS_WITH ( (NEW(NumVal)(5))->mult_with(NEW(BoolVal)(false)), "input is not a number" );
    CHECK_THROWS_WITH ( (NEW(BoolVal)(false))->mult_with(NEW(BoolVal)(false)), "cannot multiply booleans" );
    
    CHECK_THROWS_WITH( (NEW(FunVal)("x", NEW(MultExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x")), Env::emptyenv)) ->mult_with(NEW(NumVal)(4)), "cannot multiply functions" );
}

TEST_CASE( "value to_expr" ) {
    CHECK( (NEW(NumVal)(5))->to_expr()->equals(NEW(NumExpr)(5)) );
    CHECK( (NEW(BoolVal)(true))->to_expr()->equals(NEW(BoolExpr)(true)) );
    CHECK( (NEW(BoolVal)(false))->to_expr()->equals(NEW(BoolExpr)(false)) );
    
    CHECK( (NEW(FunVal)("x", NEW(MultExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x")), Env::emptyenv))
          ->to_expr()->equals(NEW(FunExpr)("x", NEW(MultExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x")))) );
}

TEST_CASE( "value to_string" ) {
    CHECK( (NEW(NumVal)(5))->to_string() == "5" );
    CHECK( (NEW(BoolVal)(true))->to_string() == "_true" );
    CHECK( (NEW(BoolVal)(false))->to_string() == "_false" );
    
    CHECK( (NEW(FunVal)("x", NEW(MultExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x")), Env::emptyenv))
          ->to_string() == "[FUNCTION]" );
}
