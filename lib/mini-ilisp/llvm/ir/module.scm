(import "../../../list.scm" :as List)
(import "../../../data/struct.scm" :names struct)
(import "../../../predicate.scm" :names =? boolean? byte-vector? character? integer? list? list-of? null? string?)
(import "../../../string.scm" :as String)
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
