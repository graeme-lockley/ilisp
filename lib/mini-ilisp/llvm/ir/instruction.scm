(import "../../../data/struct.scm" :names struct)
(import "../../../data/union.scm" :names union)
(import "../../../predicate.scm" :names =? boolean? byte-vector? character? integer? list? list-of? null? string?)
(import "../../../list.scm" :as List)
(import "../../../string.scm" :as String)

(import "./operand.scm" :as Operand)
(import "./type.scm" :as Type)

(struct Alloca
    (return Operand.LocalReference?)
    (align integer?)
)

(struct Br
    (label string?)
)

(struct Call
    (return Operand.LocalReference?)
    (name string?)
    (arguments (list-of? Operand.Operand?))
)

(struct CallVoid
    (name string?)
    (arguments (list-of? Operand.Operand?))
)

(struct CondBr
    (op Operand.Operand?)
    (labels (list-of? string?))
)

(struct ICmp
    (return Operand.LocalReference?)
    (op iop?)
    (op1 Operand.Operand?)
    (op2 Operand.Operand?)
)

(const iop?
    (or? (=? 'ne)
    )
)

(struct Label
    (name string?)
)

(struct Load
    (return Operand.LocalReference?)
    (value Operand.Operand?)
    (align number?)
)

(struct Phi
    (return Operand.LocalReference?)
    (values (list-of? Operand.LocalReference?))
)

(struct Ret
    (value Operand.Operand?)
)

(struct Store
    (value Operand.Operand?)
    (address (or? Operand.GlobalReference? Operand.LocalReference?))
    (align number?)
)

(union Instruction
    Alloca? Br? Call? CallVoid? CondBr? ICmp? Label? Load? Phi? Ret? Store?
)

(const (instruction->string instruction)
    (if (Alloca? instruction)
            (str
                (Operand.LocalReference-name (Alloca-return instruction))
                " = alloca "
                (Type.type->string (Type.dereference (Operand.LocalReference-type (Alloca-return instruction))))
                (if (= 0 (Alloca-align instruction)) "" (str ", align " (Alloca-align instruction)))
            )
        (Br? instruction)
            (str
                "br label %"
                (Label-name instruction)
            )
        (Call? instruction)
            (str 
                (Operand.LocalReference-name (Call-return instruction))
                " = call "
                (Type.type->string (Operand.LocalReference-type (Call-return instruction)))
                " "
                (Call-name instruction)
                "("
                (String.interpolate-with (List.map (Call-arguments instruction) Operand.typed-operand->string) ", ")
                ")"
            )
        (CallVoid? instruction)
            (str 
                "call void "
                (CallVoid-name instruction)
                "("
                (String.interpolate-with (List.map (CallVoid-arguments instruction) Operand.typed-operand->string) ", ")
                ")"
            )
        (CondBr? instruction)
            (str
                "br "
                (Operand.typed-operand->string (CondBr-op instruction))
                (String.interpolate-with (List.map (CondBr-labels instruction) (proc (l) (str ", label %" l))) "")
            )
        (ICmp? instruction)
            (str
                (Operand.LocalReference-name (ICmp-return instruction))
                " = icmp "
                (ICmp-op instruction)
                " "
                (Operand.typed-operand->string (ICmp-op1 instruction))
                ", "
                (Operand.untyped-operand->string (ICmp-op2 instruction))
            )
        (Label? instruction)
            (str
                (Label-name instruction)
                ":"
            )
        (Load? instruction)
            (str
                (Operand.LocalReference-name (Load-return instruction))
                " = load "
                (Type.type->string (Operand.LocalReference-type (Load-return instruction)))
                ", "
                (Operand.typed-operand->string (Load-value instruction))
                (if (= 0 (Load-align instruction)) "" (str ", align " (Load-align instruction)))
            )
        (Phi? instruction)
            (str
                (Operand.LocalReference-name (Phi-return instruction))
                " = phi "
                (Type.type->string (Operand.LocalReference-type (Phi-return instruction)))
                " "
                (String.interpolate-with (List.map (Phi-values instruction)
                    (proc (tv) 
                        (str
                            "[ "
                            (Operand.untyped-operand->string tv)
                            ", %"
                            (Operand.LocalReference-block-name tv)
                            " ]"
                        )
                    )) ", ")
            )
        (Ret? instruction)
            (str
                "ret "
                (Operand.typed-operand->string (Ret-value instruction))
                ""
            )
        (Store? instruction)
            (str
                "store "
                (Operand.typed-operand->string (Store-value instruction))
                ", "
                (Operand.typed-operand->string (Store-address instruction))
                (if (= 0 (Store-align instruction)) "" (str ", align " (Store-align instruction)))
            )
    )
)
