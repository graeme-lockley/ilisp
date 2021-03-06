(import "../../../data/struct.scm" :names struct)
(import "../../../data/union.scm" :names union)
(import "../../../list.scm" :as List)
(import "../../../predicate.scm" :names =? boolean? byte-vector? character? integer? list? list-of? null? string?)
(import "../../../string.scm" :as String)

(import "./type.scm" :names Array i8 Type? type->string)

(struct LocalReference
    (name string?)
    (type Type?)
    (block-name string?)
)

(struct GlobalReference
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

(struct CNull
    (type Type?)
)

(struct CInt
    (bits number?)
    (value number?)
)

(struct CArray
    (element-type Type?)
    (elements (list-of? Constant?))
)

(union Operand
    Constant? GetElementPtr? GlobalReference? LocalReference? 
)

(const (type-of operand)
    (if (LocalReference? operand)
            (LocalReference-type operand)
        (GlobalReference? operand)
            (GlobalReference-type operand)
        (GetElementPtr? operand)
            (GetElementPtr-type operand)
        (CInt? operand)
            (Type.Integer (CInt-bits operand))
        (CArray operand)
            (Type.Array (count (CArray-elements operand)) (CArray-element-type operand))
    )
)

(union Constant
    CNull? CInt? CArray?
)

(const (string->carray s)
    (const (character->int c) (+ 0 c))

    (CArray 
        i8 
        (concat (List.map (String.string->list s) (proc (c) (CInt 8 (character->int c)))) (list (CInt 8 0)))
    )
)

(const (untyped-operand->string operand)
    (if (CNull? operand) "null"
        (LocalReference? operand) (LocalReference-name operand)
        (GlobalReference? operand) (GlobalReference-name operand)
        (CInt? operand) (str (CInt-value operand))
        (CArray? operand) (carray->string operand #f)
        (GetElementPtr? operand) (get-element-ptr->string operand #f)
    )
)

(const (typed-operand->string operand)
    (if (CNull? operand) (str (type->string (CNull-type operand)) " null")
        (LocalReference? operand) (str (type->string (LocalReference-type operand)) " " (LocalReference-name operand))
        (GlobalReference? operand) (str (type->string (GlobalReference-type operand)) " " (GlobalReference-name operand))
        (CInt? operand) (str "i" (CInt-bits operand) " " (CInt-value operand))
        (CArray? operand) (carray->string operand #t)
        (GetElementPtr? operand) (get-element-ptr->string operand #t)
    )
)

(const- (carray->string operand show-type)
    (str 
        (if show-type (str (type->string (Array (count (CArray-elements operand)) (CArray-element-type operand))) " ") "")
        "[" 
        (String.interpolate-with (List.map (CArray-elements operand) typed-operand->string) ", ") 
        "]"
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
