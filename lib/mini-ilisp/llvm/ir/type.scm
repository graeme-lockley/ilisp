(import "../../../data/struct.scm" :names struct)
(import "../../../data/union.scm" :names union)
(import "../../../list.scm" :as List)
(import "../../../predicate.scm" :names =? boolean? byte-vector? character? integer? list? list-of? null? string?)
(import "../../../string.scm" :as String)

(struct Void
)

(struct Integer
    (bits integer?)
)

(struct Pointer
    (type Type?)
)

(struct Function
    (arguments (list-of? Type?))
    (result Type?)
    (is-var-arg boolean?)
)

(struct Structure
    (is-packed boolean?)
    (elements (list-of? Type?))
)

(struct Array
    (size integer?)
    (element Type?)
)

(struct Reference
    (name string?)
)

(union Type
    Void? Integer? Pointer? Function? Structure? Array? Reference?
)

(const void (Void))

(const i1 (Integer 1))

(const i8 (Integer 8))

(const i32 (Integer 32))

(const i64 (Integer 64))

(const (dereference t)
    (if (Pointer? t) (Pointer-type t)
        t
    )
)

(const- (riap l e)
    (if (null? l) (list e)
        (pair (car l) (riap (cdr l) e))
    )
)

(const (type->string type)
    (if (Void? type) "void"
        (Integer? type) (str "i" (Integer-bits type))
        (Pointer? type) (str (type->string (Pointer-type type)) "*")
        (Function? type) 
            (if (Function-is-var-arg type)                
                (str (type->string (Function-result type)) " (" (String.interpolate-with (riap (List.map (Function-arguments type) type->string) "...") ", ") ")")
                (str (type->string (Function-result type)) " (" (String.interpolate-with (List.map (Function-arguments type) type->string) ", ") ")")
            )
        (Structure? type)
            (if (Structure-is-packed type)
                (str "<{" (String.interpolate-with (List.map (Structure-elements type) type->string) ", ") "}>")
                (str "{" (String.interpolate-with (List.map (Structure-elements type) type->string) ", ") "}")
            )
        (Array? type)
            (str "[" (Array-size type) " x " (type->string (Array-element type)) "]")
        (Reference? type)
            (Reference-name type)
        (raise 'TODO-type->string type)
    )
)