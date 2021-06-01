(import "../../../data/struct.scm" :names struct)
(import "../../../predicate.scm" :names =? boolean? byte-vector? character? integer? list? list-of? null? string?)
(import "./type.scm" :names Type? type->string)

(struct IdentifiedType
    (name string?)
    (type Type?)
)

(const (identified-type->string identified-type)
    (str (IdentifiedType-name identified-type) " = type " (type->string (IdentifiedType-type identified-type)))
)