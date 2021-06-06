(import "../../../list.scm" :as List)
(import "../../../string.scm" :as String)
(import "../../../unit.scm" :as Unit)

(import "./instruction.scm" :as Instruction)
(import "./operand.scm" :as Operand)
(import "./type.scm" :as Type)

(Unit.test "instruction->string Call"
    (Unit.assert-equals 
        (Instruction.instruction->string (Instruction.Call (Operand.LocalReference "%1" Type.i32) "@_fred" ()))
        "  %1 = call i32 @_fred()\n"
    )

    (Unit.assert-equals 
        (Instruction.instruction->string (Instruction.Call (Operand.LocalReference "%2" Type.i32) "@_fred" (list (Operand.LocalReference "%1" (Type.Pointer (Type.Reference "%struct.Value"))))))
        "  %2 = call i32 @_fred(%struct.Value* %1)\n"
    )
)

(Unit.test "instruction->string CallVoid"
    (Unit.assert-equals 
        (Instruction.instruction->string (Instruction.CallVoid "@_print_newline" ()))
        "  call void @_print_newline()\n"
    )

    (Unit.assert-equals 
        (Instruction.instruction->string (Instruction.CallVoid "@_print_value" (list (Operand.LocalReference "%1" (Type.Pointer (Type.Reference "%struct.Value"))))))
        "  call void @_print_value(%struct.Value* %1)\n"
    )
)

(Unit.test "instruction->string Ret"
    (Unit.assert-equals 
        (Instruction.instruction->string (Instruction.Ret (Operand.LocalReference "%1" Type.i32)))
        "  ret i32 %1\n"
    )
)
