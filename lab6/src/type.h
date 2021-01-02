#ifndef TYPESYSTEM_H
#define TYPESYSTEM_H

#include "./pch.h"

using namespace std;

enum Type {
    TYPE_INT,
    TYPE_CHAR,
    TYPE_BOOL,
    TYPE_STRING,
    TYPE_VOID,
    TYPE_INT_CONST,
    TYPE_CHAR_CONST,
    TYPE_INT_POINTER,
    TYPE_CHAR_POINTER,
    TYPE_COMPOSE_STRUCT,
};

#endif