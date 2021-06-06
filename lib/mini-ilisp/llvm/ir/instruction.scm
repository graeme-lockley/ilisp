(import "../../../data/struct.scm" :names struct)
(import "../../../data/union.scm" :names union)
(import "../../../predicate.scm" :names =? boolean? byte-vector? character? integer? list? list-of? null? string?)
(import "../../../list.scm" :as List)
(import "../../../string.scm" :as String)

(import "./operand.scm" :as Operand)
(import "./type.scm" :as Type)

(struct Call
    (return Operand.LocalReference?)
    (name string?)
    (arguments (list-of? Operand.Operand?))
)

(struct CallVoid
    (name string?)
    (arguments (list-of? Operand.Operand?))
)

(const (instruction->string instruction)
    (if (Call? instruction)
            (str 
                "  "
                (Operand.LocalReference-name (Call-return instruction))
                " = call "
                (Type.type->string (Operand.LocalReference-type (Call-return instruction)))
                " "
                (Call-name instruction)
                "("
                (String.interpolate-with (List.map (Call-arguments instruction) Operand.typed-operand->string) ", ")
                ")\n"
            )
        (CallVoid? instruction)
            (str 
                "  call void "
                (CallVoid-name instruction)
                "("
                (String.interpolate-with (List.map (CallVoid-arguments instruction) Operand.typed-operand->string) ", ")
                ")\n"
            )
    )
)
