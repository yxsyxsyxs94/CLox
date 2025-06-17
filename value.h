#pragma once
#include "common.h"
typedef enum {
	VAL_BOOL,
	VAL_NIL,
	VAL_NUMBER,
} ValueType;

struct  Value {
	ValueType type;
	union {
		bool boolean;
		double number;
	} as;

	// ������������Value
	Value(double num) : type(VAL_NUMBER) {
		as.number = num;
	}

	// ����bool����Value
	Value(bool b) : type(VAL_BOOL) {
		as.boolean = b;
	}

	// ����nil����Value
	Value() : type(VAL_NIL) {
		as.number = 0;
	}
};

typedef struct {
	int capacity;
	int count;
	Value* values;
} ValueArray;

#define IS_BOOL(value)    ((value).type == VAL_BOOL)
#define IS_NIL(value)     ((value).type == VAL_NIL)
#define IS_NUMBER(value)  ((value).type == VAL_NUMBER)
//����
#define AS_BOOL(value)    ((value).as.boolean)
#define AS_NUMBER(value)  ((value).as.number)
//װ��
#define NUMBER_VAL(value) Value(value)
#define BOOL_VAL(value) Value(value)
#define NIL_VAL Value()

void initValueArray(ValueArray* array);
void writeValueArray(ValueArray* array, Value value);
void freeValueArray(ValueArray* array);
void printValue(Value value);
