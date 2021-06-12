(import "../../../list.scm" :as List)
(import "../../../string.scm" :as String)
(import "../../../unit.scm" :as Unit)

(import "./instruction.scm" :as Instruction)
(import "./operand.scm" :as Operand)
(import "./type.scm" :as Type)

(const struct-value (Type.Reference "%struct.Value"))
(const struct-value-pointer (Type.Pointer struct-value))
(const struct-value-pointer-pointer (Type.Pointer struct-value-pointer))

(Unit.test "instruction->string Call"
    (Unit.assert-equals 
        (Instruction.instruction->string (Instruction.Call (Operand.LocalReference "%1" Type.i32) "@_fred" ()))
        "%1 = call i32 @_fred()"
    )

    (Unit.assert-equals 
        (Instruction.instruction->string (Instruction.Call (Operand.LocalReference "%2" Type.i32) "@_fred" (list (Operand.LocalReference "%1" struct-value-pointer))))
        "%2 = call i32 @_fred(%struct.Value* %1)"
    )
)

(Unit.test "instruction->string CallVoid"
    (Unit.assert-equals 
        (Instruction.instruction->string (Instruction.CallVoid "@_print_newline" ()))
        "call void @_print_newline()"
    )

    (Unit.assert-equals 
        (Instruction.instruction->string (Instruction.CallVoid "@_print_value" (list (Operand.LocalReference "%1" struct-value-pointer))))
        "call void @_print_value(%struct.Value* %1)"
    )
)

(Unit.test "instruction->string Load"
    (Unit.assert-equals 
        (Instruction.instruction->string (Instruction.Load (Operand.LocalReference "%6" struct-value-pointer) (Operand.LocalReference "@_VFalse" struct-value-pointer-pointer) 8))
        "%6 = load %struct.Value*, %struct.Value** @_VFalse, align 8"
    )
)

(Unit.test "instruction->string ICmp"
    (Unit.assert-equals 
        (Instruction.instruction->string (Instruction.ICmp (Operand.LocalReference "%28" Type.i1) 'ne (Operand.LocalReference "%26" struct-value-pointer) (Operand.LocalReference "%27" struct-value-pointer)))
        "%28 = icmp ne %struct.Value* %26, %27"
    )
)

(Unit.test "instruction->string Ret"
    (Unit.assert-equals 
        (Instruction.instruction->string (Instruction.Ret (Operand.LocalReference "%1" Type.i32)))
        "ret i32 %1"
    )
)
