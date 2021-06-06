(import "../../../data/struct.scm" :names struct)
(import "../../../data/union.scm" :names union)
(import "../../../predicate.scm" :names =? boolean? byte-vector? character? integer? list? list-of? null? string?)
(import "../../../list.scm" :as List)
(import "../../../string.scm" :as String)

(import "./operand.scm" :names Operand?)

(struct CallVoid
    (name string?)
    (arguments (list-of? Operand?))
)

(const (instruction->string instruction)
    (if (CallVoid? instruction)
            (str 
                "  call void "
                (CallVoid-name instruction)
                "("
                (String.interpolate-with (List.map (CallVoid-arguments instruction) Operand.typed-operand->string) ", ")
                ")\n"
            )
    )
)
