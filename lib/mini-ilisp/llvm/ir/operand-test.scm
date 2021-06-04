(import "../../../unit.scm" :as Unit)

(import "./type.scm" :as Type)
(import "./operand.scm" :as Operand)

(Unit.test "operand->string local reference"
    (const operand (Operand.LocalReference "%1" (Type.Pointer (Type.Reference "%struct.Value"))))

    (Unit.assert-equals (Operand.untyped-operand->string operand) "%1")
    (Unit.assert-equals (Operand.typed-operand->string operand) "%struct.Value* %1")
)
