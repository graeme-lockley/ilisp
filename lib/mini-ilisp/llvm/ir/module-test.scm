(import "../../../unit.scm" :as Unit)

(import "./type.scm" :as Type)
(import "./module.scm" :as Module)

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
