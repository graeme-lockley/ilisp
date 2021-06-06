(import "../../../unit.scm" :as Unit)

(import "./module.scm" :as Module)
(import "./operand.scm" :as Operand)
(import "./type.scm" :as Type)

(Unit.test "identified-type->string"
    (Unit.assert-equals 
        (Module.identified-type->string (Module.IdentifiedType "%struct.Value" (Type.Structure #f (list Type.i32 (Type.Reference "%union.anon")))))
        "%struct.Value = type {i32, %union.anon}"
    )
)

(Unit.test "external->string"
    (Unit.assert-equals 
        (Module.external->string (Module.External "@_print_ln" Type.void () #f))
        "declare void @_print_ln()"
    )

    (Unit.assert-equals 
        (Module.external->string (Module.External "@_print_ln" Type.void () #t))
        "declare void @_print_ln(...)"
    )

    (Unit.assert-equals 
        (Module.external->string (Module.External "@_print_value" Type.void (list (Type.Pointer (Type.Reference "%struct.Value"))) #f))
        "declare void @_print_value(%struct.Value*)"
    )

    (Unit.assert-equals 
        (Module.external->string (Module.External "@printf" Type.i32 (list (Type.Pointer Type.i8)) #t))
        "declare i32 @printf(i8*, ...)"
    )
)

(Unit.test "global->string"
    (Unit.assert-equals 
        (Module.global->string (Module.Global "@.str0" (Type.Array 12 Type.i8) (Operand.string->carray "hello world") #t #t #t 0))
        "@.str0 = private unnamed_addr constant [12 x i8] [i8 104, i8 101, i8 108, i8 108, i8 111, i8 32, i8 119, i8 111, i8 114, i8 108, i8 100, i8 0]"
    )

    (Unit.assert-equals 
        (Module.global->string (Module.Global "@.str0" (Type.Array 12 Type.i8) (Operand.string->carray "hello world") #t #t #t 1))
        "@.str0 = private unnamed_addr constant [12 x i8] [i8 104, i8 101, i8 108, i8 108, i8 111, i8 32, i8 119, i8 111, i8 114, i8 108, i8 100, i8 0], align 1"
    )

    (Unit.assert-equals 
        (Module.global->string (Module.Global "@.str0" (Type.Array 12 Type.i8) (Operand.string->carray "hello world") #f #t #t 1))
        "@.str0 = unnamed_addr constant [12 x i8] [i8 104, i8 101, i8 108, i8 108, i8 111, i8 32, i8 119, i8 111, i8 114, i8 108, i8 100, i8 0], align 1"
    )

    (Unit.assert-equals 
        (Module.global->string (Module.Global "@.str0" (Type.Array 12 Type.i8) (Operand.string->carray "hello world") #t #f #t 1))
        "@.str0 = private constant [12 x i8] [i8 104, i8 101, i8 108, i8 108, i8 111, i8 32, i8 119, i8 111, i8 114, i8 108, i8 100, i8 0], align 1"
    )

    (Unit.assert-equals 
        (Module.global->string (Module.Global "@.str0" (Type.Array 12 Type.i8) (Operand.string->carray "hello world") #t #t #f 1))
        "@.str0 = private unnamed_addr [12 x i8] [i8 104, i8 101, i8 108, i8 108, i8 111, i8 32, i8 119, i8 111, i8 114, i8 108, i8 100, i8 0], align 1"
    )
)