(import "../../../list.scm" :as List)
(import "../../../string.scm" :as String)
(import "../../../unit.scm" :as Unit)

(import "./type.scm" :as Type)
(import "./operand.scm" :as Operand)

(Unit.test "operand->string local reference"
    (const operand (Operand.LocalReference "%1" (Type.Pointer (Type.Reference "%struct.Value")) "Block0"))

    (Unit.assert-equals (Operand.untyped-operand->string operand) "%1")
    (Unit.assert-equals (Operand.typed-operand->string operand) "%struct.Value* %1")
)

(Unit.test "operand->string constant int"
    (const operand (Operand.CInt 8 127))

    (Unit.assert-equals (Operand.untyped-operand->string operand) "127")
    (Unit.assert-equals (Operand.typed-operand->string operand) "i8 127")
)

(Unit.test "operand->string constant string"
    (const operand (Operand.string->carray "hello world"))

    (Unit.assert-equals (Operand.untyped-operand->string operand) "[i8 104, i8 101, i8 108, i8 108, i8 111, i8 32, i8 119, i8 111, i8 114, i8 108, i8 100, i8 0]")
    (Unit.assert-equals (Operand.typed-operand->string operand) "[12 x i8] [i8 104, i8 101, i8 108, i8 108, i8 111, i8 32, i8 119, i8 111, i8 114, i8 108, i8 100, i8 0]")
)

(Unit.test "operand->string get element ptr"
    (const operand (Operand.GetElementPtr 
        #t 
        (Type.Pointer Type.i8)
        (Type.Array 12 Type.i8)
        (Operand.GlobalReference "@.str" (Type.Pointer (Type.Array 12 Type.i8)))
        (list (Operand.CInt 64 0) (Operand.CInt 64 0))
    ))

    (Unit.assert-equals (Operand.untyped-operand->string operand) "getelementptr inbounds ([12 x i8], [12 x i8]* @.str, i64 0, i64 0)")
    (Unit.assert-equals (Operand.typed-operand->string operand) "i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str, i64 0, i64 0)")
)

