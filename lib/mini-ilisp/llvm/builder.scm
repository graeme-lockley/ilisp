(import "../../data/struct.scm" :names mutable-struct)
(import "../../predicate.scm" :names =? byte-vector? character? integer? list? list-of? null? string?)

(import "./ir/module.scm" :as Module)
(import "./ir/type.scm" :names Type?)

(const (module id)
    (ModuleBuilder id ())
)

(const (function builder name return-type parameter-types)
    (FunctionBuilder builder name return-type parameter-types () 1 0)
)

(const (declare-identified-type! builder name type)
    (include-declaration builder (Module.IdentifiedType name type))
)

(const (declare-external! builder name return-type parameter-types)
    (include-declaration builder (Module.External name return-type parameter-types #f))
)

(const- (include-declaration builder declarations)
    (ModuleBuilder-declarations! builder (pair declarations (ModuleBuilder-declarations builder)))
)

(mutable-struct ModuleBuilder
    (id string?)
    (declarations list?)
)

(mutable-struct FunctionBuilder
    (module-builder ModuleBuilder?)
    (name string?)
    (return-type Type?)
    (parameter-types (list-of? Type?))
    (instructions list?)
    (label-count integer?)
    (register-count integer?)
)
