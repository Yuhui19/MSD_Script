//
//  expr.hpp
//  
//
//  Created by Yuhui on 1/22/20.
//  Copyright © 2020 Yuhui. All rights reserved.
//

#ifndef expr_hpp
#define expr_hpp

#include <stdio.h>
#include <string>
#include "value.hpp"
#include "pointer.hpp"

class Val;
class Env;

class Expr ENABLE_THIS(Expr){
public:
    virtual bool equals(PTR(Expr) e) = 0;
    
    //For counting the value of expression
    virtual PTR(Val) to_value(PTR(Env) env) = 0;
    
    //For substituting a number with a variable by its value
    virtual PTR(Expr) subst(std::string var, PTR(Val) val) = 0;
    
    //For checking if an expression contains variable, both decided or undecided.
    virtual bool containsVariables() = 0;
    
    //For optimizing an expression, also applied in --opt mode
    virtual PTR(Expr) optimize() = 0;
    
    //For both step and optimize an expression in --step mode
    virtual void step_interp() = 0;
    
    //For making an expression to a string which can be printed out
    virtual std::string to_string() = 0;
};

class NumExpr : public Expr {
public:
    int num;

    NumExpr(int num);
    bool equals(PTR(Expr) e);
    PTR(Val) to_value(PTR(Env) env);
    PTR(Expr) subst(std::string var, PTR(Val) val);
    bool containsVariables();
    PTR(Expr) optimize();
    void step_interp();
    std::string to_string();
};

class AddExpr : public Expr {
public:
    PTR(Expr) lhs;
    PTR(Expr) rhs;
    
    AddExpr(PTR(Expr) lhs, PTR(Expr) rhs);
    bool equals(PTR(Expr) e);
    PTR(Val) to_value(PTR(Env) env);
    PTR(Expr) subst(std::string var, PTR(Val) val);
    bool containsVariables();
    PTR(Expr) optimize();
    void step_interp();
    std::string to_string();
};

class MultExpr : public Expr {
public:
    PTR(Expr) lhs;
    PTR(Expr) rhs;
    
    MultExpr(PTR(Expr) lhs, PTR(Expr) rhs);
    bool equals(PTR(Expr) e);
    PTR(Val) to_value(PTR(Env) env);
    PTR(Expr) subst(std::string var, PTR(Val) val);
    bool containsVariables();
    PTR(Expr) optimize();
    void step_interp();
    std::string to_string();
};

class VarExpr : public Expr {
public:
    std::string name;

    VarExpr(std::string name);
    bool equals(PTR(Expr) e);
    PTR(Val) to_value(PTR(Env) env);
    PTR(Expr) subst(std::string var, PTR(Val) val);
    bool containsVariables();
    PTR(Expr) optimize();
    void step_interp();
    std::string to_string();
};

class BoolExpr : public Expr {
public:
    bool rep;
  
    BoolExpr(bool rep);
    bool equals(PTR(Expr) e);
    PTR(Val) to_value(PTR(Env) env);
    PTR(Expr) subst(std::string var, PTR(Val) val);
    bool containsVariables();
    PTR(Expr) optimize();
    void step_interp();
    std::string to_string();
};

class LetExpr : public Expr {
public:
    std::string name;
    PTR(Expr) rhs;
    PTR(Expr) expr;
    
    LetExpr(std::string name, PTR(Expr) rhs, PTR(Expr) expr);
    bool equals(PTR(Expr) e);
    PTR(Val) to_value(PTR(Env) env);
    PTR(Expr) subst(std::string var, PTR(Val) val);
    bool containsVariables();
    PTR(Expr) optimize();
    void step_interp();
    std::string to_string();
};

class EqualExpr : public Expr {
public:
    PTR(Expr) lhs;
    PTR(Expr) rhs;
    
    EqualExpr(PTR(Expr) lhs, PTR(Expr) rhs);
    bool equals(PTR(Expr) e);
    PTR(Val) to_value(PTR(Env) env);
    PTR(Expr) subst(std::string var, PTR(Val) val);
    bool containsVariables();
    PTR(Expr) optimize();
    void step_interp();
    std::string to_string();
};

class IfExpr : public Expr {
public:
    PTR(Expr) if_part;
    PTR(Expr) then_part;
    PTR(Expr) else_part;
    
    IfExpr(PTR(Expr) if_part, PTR(Expr) then_part, PTR(Expr) else_part);
    bool equals(PTR(Expr) e);
    PTR(Val) to_value(PTR(Env) env);
    PTR(Expr) subst(std::string var, PTR(Val) val);
    bool containsVariables();
    PTR(Expr) optimize();
    void step_interp();
    std::string to_string();
};

class FunExpr : public Expr {
public:
    std::string formal_arg;
    PTR(Expr) body;
    
    FunExpr(std::string formal_arg, PTR(Expr) body);
    bool equals(PTR(Expr) e);
    PTR(Val) to_value(PTR(Env) env);
    PTR(Expr) subst(std::string var, PTR(Val) val);
    bool containsVariables();
    PTR(Expr) optimize();
    void step_interp();
    std::string to_string();
};

class CallFunExpr : public Expr {
public:
    PTR(Expr) to_be_called;
    PTR(Expr) actual_arg;
    
    CallFunExpr(PTR(Expr) to_be_called, PTR(Expr) actual_arg);
    bool equals(PTR(Expr) e);
    PTR(Val) to_value(PTR(Env) env);
    PTR(Expr) subst(std::string var, PTR(Val) val);
    bool containsVariables();
    PTR(Expr) optimize();
    void step_interp();
    std::string to_string();
};

#endif /* expr_hpp */

