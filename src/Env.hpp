

#ifndef Env_h
#define Env_h

#include <string>
#include "pointer.hpp"
#include "value.hpp"


class Val;

class Env {
public:
    
    static PTR(Env) emptyenv;
    
    virtual PTR(Val) lookup(std::string find_name) = 0;
    
    virtual bool equals(PTR(Env) env) = 0;
};

class EmptyEnv : public Env {
public:
    PTR(Val) lookup(std::string find_name);
    bool equals(PTR(Env) env);
};

class ExtendedEnv : public Env {
public:
    std::string name;
    PTR(Val) val;
    PTR(Env) rest;
    
    ExtendedEnv(std::string name, PTR(Val) val, PTR(Env) env);
    PTR(Val) lookup(std::string find_name);
    bool equals(PTR(Env) env);
};


#endif /* Env_h */
