(import "../../../list.scm" :as List)
(import "../../../data/struct.scm" :names struct)
(import "../../../predicate.scm" :names =? boolean? byte-vector? character? integer? list? list-of? null? string?)
(import "../../../string.scm" :as String)

(import "./instruction.scm" :as Instruction)
(import "./operand.scm" :as Operand)
(import "./type.scm" :names Type? type->string)

(struct IdentifiedType
    (name string?)
    (type Type?)
)

(struct External
    (name string?)
    (return-type Type?)
    (parameter-types (list-of? Type?))
    (is-var-arg boolean?)
)

(struct Global
    (name string?)
    (type Type?)
    (value Operand.Constant?)

    (private boolean?)
    (unnamed-addr boolean?)
    (constant boolean?)
    (align number?)
)

(struct Function
    (name string?)
    (return-type Type?)
    (parameter-types (list-of? Type?))
    (instructions (list-of? Instruction.Instruction?))
)

(const (identified-type->string identified-type)
    (str (IdentifiedType-name identified-type) " = type " (type->string (IdentifiedType-type identified-type)))
)

(const- (riap l e)
    (if (null? l) (list e)
        (pair (car l) (riap (cdr l) e))
    )
)

(const (external->string external)
    (const parameters
        (if (External-is-var-arg external)
                (String.interpolate-with (riap (List.map (External-parameter-types external) type->string) "...") ", ")
            (String.interpolate-with (List.map (External-parameter-types external) type->string) ", ")            
        )
    )

    (str "declare " (type->string (External-return-type external)) " " (External-name external) "(" parameters ")")
)

(const (global->string global)
    (str
        (Global-name global)
        " = "
        (if (Global-private global) "private " "")
        (if (Global-unnamed-addr global) "unnamed_addr " "")
        (if (Global-constant global) "constant " "")
        (type->string (Global-type global))
        " "
        (Operand.untyped-operand->string (Global-value global))
        (if (= 0 (Global-align global)) "" (str ", align " (Global-align global)))
    )
)

(const (function->string function)
    (str
        "define "
        (Type.type->string (Function-return-type function))
        " "
        (Function-name function)
        "() {\n"
        (String.interpolate-with (List.map (Function-instructions function) Instruction.instruction->string) "")
        "}\n"
    )
)