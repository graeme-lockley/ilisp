(import "../../../unit.scm" :as Unit)

(import "./type.scm" :names Array Function i1 i32 Pointer Structure type->string void)

(Unit.test "type->string Void"
    (Unit.assert-equals (type->string void) "void")
)

(Unit.test "type->string Integer"
    (Unit.assert-equals (type->string i1) "i1")
    (Unit.assert-equals (type->string i32) "i32")
)

(Unit.test "type->string Pointer"
    (Unit.assert-equals (type->string (Pointer i1)) "i1*")
    (Unit.assert-equals (type->string (Pointer i32)) "i32*")
    (Unit.assert-equals (type->string (Pointer (Pointer void))) "void**")
)

(Unit.test "type->string Function"
    (Unit.assert-equals (type->string (Function () i1 #f)) "i1 ()")
    (Unit.assert-equals (type->string (Function (list (Pointer i32) (Pointer (Pointer void))) i1 #f)) "i1 (i32*, void**)")
    (Unit.assert-equals (type->string (Function () i1 #t)) "i1 (...)")
    (Unit.assert-equals (type->string (Function (list (Pointer i32) (Pointer (Pointer void))) i1 #t)) "i1 (i32*, void**, ...)")
)

(Unit.test "type->string Structure"
    (Unit.assert-equals (type->string (Structure #f ())) "{}")
    (Unit.assert-equals (type->string (Structure #f (list i1 i32))) "{i1, i32}")
    (Unit.assert-equals (type->string (Structure #t ())) "<{}>")
    (Unit.assert-equals (type->string (Structure #t (list i1 i32))) "<{i1, i32}>")
)

(Unit.test "type->string Array"
    (Unit.assert-equals (type->string (Array 4 (Pointer i32))) "[4 x i32*]")
)
