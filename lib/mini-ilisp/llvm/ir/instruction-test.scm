(import "../../../list.scm" :as List)
(import "../../../string.scm" :as String)
(import "../../../unit.scm" :as Unit)

(import "./instruction.scm" :as Instruction)
(import "./operand.scm" :as Operand)
(import "./type.scm" :as Type)

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
