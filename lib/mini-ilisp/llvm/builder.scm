(import "../../data/struct.scm" :names mutable-struct)
(import "../../predicate.scm" :names =? byte-vector? character? integer? list? list-of? null? string?)

(import "./ir/instruction.scm" :as Instruction)
(import "./ir/module.scm" :as Module)
(import "./ir/operand.scm" :as Operand)
(import "./ir/type.scm" :names Array i8 Type?)
(import "./ir/type.scm" :as Type)

(const (module id)
    (ModuleBuilder id () 0)
)

(const (function builder name return-type parameter-types)
    (FunctionBuilder builder name return-type parameter-types () 1 1)
)

(const (declare-identified-type! builder name type)
    (include-declaration! builder (Module.IdentifiedType name type))
)

(const (declare-external! builder name return-type parameter-types)
    (include-declaration! builder (Module.External name return-type parameter-types #f))
)

(const (declare-string-literal! builder value)
    (const builder' (if (FunctionBuilder? builder) (FunctionBuilder-module-builder builder) builder))

    (const name (str "@.str" (ModuleBuilder-literal-string-count builder')))

    (ModuleBuilder-literal-string-count! builder' (+ 1 (ModuleBuilder-literal-string-count builder')))
    (include-declaration! builder' (Module.Global name (Array (+ (count value) 1) i8) (Operand.string->carray value) #t #t #t 1))

    (Operand.GetElementPtr 
        #t 
        (Type.Pointer Type.i8) 
        (Type.Array (+ (count value) 1) Type.i8) 
        (Operand.LocalReference name (Type.Pointer (Type.Array (+ (count value) 1) Type.i8)))
        (list (Operand.CInt 64 0) (Operand.CInt 64 0))
    )
)

(const- (include-declaration! builder declaration)
    (ModuleBuilder-declarations! builder (pair declaration (ModuleBuilder-declarations builder)))
)

(const (call-void! builder name arguments)
    (include-instruction! builder (Instruction.CallVoid name arguments))
)

(const (call! builder name return-type arguments)
    (const var (Operand.LocalReference (new-variable! builder) return-type))

    (include-instruction! builder (Instruction.Call var name arguments))

    var
)

(const- (include-instruction! builder instruction)
    ;; (println (Instruction.instruction->string instruction))

    (FunctionBuilder-instructions! builder (pair instruction (FunctionBuilder-instructions builder)))
)

(const- (new-variable! builder)
    (const variable (str "%" (FunctionBuilder-register-count builder)))

    (FunctionBuilder-register-count! builder (+ (FunctionBuilder-register-count builder) 1))

    variable
)

(mutable-struct ModuleBuilder
    (id string?)
    (declarations list?)
    (literal-string-count number?)
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
