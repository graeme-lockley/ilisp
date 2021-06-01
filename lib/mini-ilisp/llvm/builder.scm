(import "../../data/struct.scm" :names mutable-struct)
(import "../../predicate.scm" :names =? byte-vector? character? integer? list? null? string?)

(import "./ir/module.scm" :as Module)

(const (mk id)
    (Builder id ())
)

(const (declare-identified-type builder name type)
    (Builder-declarations! builder (pair (Module.IdentifiedType name type) (Builder-declarations builder)))
)

(mutable-struct Builder
    (id string?)
    (declarations list?)
)
