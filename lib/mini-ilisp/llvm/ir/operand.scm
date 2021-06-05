(import "../../../data/struct.scm" :names struct)
(import "../../../data/union.scm" :names union)
(import "../../../list.scm" :as List)
(import "../../../predicate.scm" :names =? boolean? byte-vector? character? integer? list? list-of? null? string?)
(import "../../../string.scm" :as String)

(import "./type.scm" :names Type? type->string)

(struct LocalReference
    (name string?)
    (type Type?)
)

(struct GetElementPtr
    (inbounds boolean?)
    (type Type?)
    (element-type Type?)
    (address Operand?)
    (indices (list-of? Constant?))
)

(struct CInt
    (bits number?)
    (value number?)
)

(union Operand
    LocalReference? GetElementPtr? Constant?
)

(union Constant
    CInt?
)

(const (untyped-operand->string operand)
    (if (LocalReference? operand) (LocalReference-name operand)
        (CInt? operand) (str (CInt-value operand))
        (GetElementPtr? operand) (get-element-ptr->string operand #f)
    )
)

(const (typed-operand->string operand)
    (if (LocalReference? operand) (str (type->string (LocalReference-type operand)) " " (LocalReference-name operand))
        (CInt? operand) (str "i" (CInt-bits operand) " " (CInt-value operand))
        (GetElementPtr? operand) (get-element-ptr->string operand #t)
    )
)

(const- (get-element-ptr->string operand show-type)
    (str
        (if show-type (str (type->string (GetElementPtr-type operand)) " ") "")
        "getelementptr"
        (if (GetElementPtr-inbounds operand) " inbounds " " ")
        "("
        (type->string (GetElementPtr-element-type operand))
        ", "
        (typed-operand->string (GetElementPtr-address operand))
        (*builtin*.apply str
            (List.map (GetElementPtr-indices operand) (proc (i) (str ", " (typed-operand->string i))))
        )
        ")"
    )
)
