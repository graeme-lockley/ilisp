#include <stdio.h>

#define INT_VALUE 0
#define STR_VALUE 1

struct Value
{
	char type;
	union
	{
		int number;
		char *str;
	};
};

void print_value(struct Value *value)
{
	switch (value->type)
	{
	case INT_VALUE:
		printf("%d", value->number);
		break;

	case STR_VALUE:
		printf("%s", value->str);
		break;
	}
}

int main()
{
	struct Value v;
	v.type = STR_VALUE;
	v.str = "hello world";

	print_value(&v);
	printf("\n");
}
