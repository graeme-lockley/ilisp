The purpose of this note is to write down strategies used in the compiler.  Now I have an ability to create closures and I know how to wrap native procedures.


# Closures

The following declarations define a closure and a frame

```llvm
%struct.Closure = type { i8*, %struct.Frame* }
%struct.Frame = type { %struct.Frame*, i8* }
```

Now as this stands these structures are different to what is represented by `Value`.  I would like to unify `%struct.Closure` and `%struct.Frame` with `Value` allowing the garbage collector to always deal with tagged values.  Combining we then get

```c
struct Value
{
    int tag;
    union
    {
        int boolean;
        int integer;
        char *string;
        struct Pair {
            struct Value *car;
            struct Value *cdr;
        } pair;
        struct Vector {
            int length;
            struct Value **items;
        } vector;
        struct NativeClosure {
            void *procedure;
            int number_arguments;
            void *native_procedure;
        } native_closure;
        struct DynamicClosure {
            void *procedure;
            int number_arguments;
            struct Value *frame;
        } dynamic_closure;
    };
};
```

Some notes:

- Separated closures out into a native and a dynamic closure.  A *Native Closure* is a closure over a procedure where the procedure does not have any free variables.  As a result this procudure has been lifted and is now at the top level and does not accept a frame pointer as its first argument.  A *Dynamic Closure* does have free variables and therefore needs to be passed a frame pointer whenever it is invoked.
- The *Native Closure*'s `procedure` is a pointer to a wrap function whilst the `native_procedure` is a pointer to the actual top level procedure.
- The number of arguments are included into both closures enabling a runtime check and a descriptive error on faliure.
- Vectors are used to implement frames.

Having thought about this I think it is necessary to take a more principled approach.  So here it is:

- First get a naieve strategy in place that will work and provide satisfactory performance.
- Optimise once it is working

Okay so, given that, this is the approach:

- Ensure that frames and closures are all represented as tagged values.  This will aid garbage collection.
- Top-level procedures will not receive a frame pointer.
- Top-level procedures will create a frame regardless of whether or not the body has any lambda or nested functions.
- Embedded procedures will receive the frame regardless of whether or not they have any free variables.
- All top-level procedures will have a closure created for them as part of a module's initialisation.
- Once this is all working then optimisation can take place.