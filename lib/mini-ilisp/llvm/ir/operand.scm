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

(struct CInt
    (bits number?)
    (value number?)
)

(const (untyped-operand->string operand)
    (if (LocalReference? operand) (LocalReference-name operand)
        (CInt? operand) (str (CInt-value operand))
    )
)

(const (typed-operand->string operand)
    (if (LocalReference? operand) (str (type->string (LocalReference-type operand)) " " (LocalReference-name operand))
        (CInt? operand) (str "i" (CInt-bits operand) " " (CInt-value operand))
    )
)
