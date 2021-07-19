(import "../../data/struct.scm" :names mutable-struct struct)
(import "../../predicate.scm" :names =? any? byte-vector? character? integer? list? list-of? map? null? string?)
(import "../../list.scm" :as List)
(import "../../map.scm" :as Map)
(import "../../string.scm" :as String)

(import "./ir/instruction.scm" :as Instruction)
(import "./ir/module.scm" :as Module)
(import "./ir/operand.scm" :as Operand)
(import "./ir/type.scm" :names Array i8 Type?)
(import "./ir/type.scm" :as Type)
(import "../environment.scm" :as Env)

(const (module id)
    (ModuleBuilder id () 0 (Env.empty))
)

(const opening-block-name "Block0")

(const- struct-value (Type.Reference "%struct.Value"))

(const- struct-value-pointer (Type.Pointer struct-value))

(const (function builder name top-level return-type parameter-types)
    (const names (names builder))
    (const names' (Env.open-scope names))
    
    (FunctionBuilder 
        builder 
        name 
        return-type 
        (if top-level parameter-types (pair struct-value-pointer parameter-types))
        () 
        1 
        ;; (+ (count parameter-types) 1) 
        (if top-level (+ (count parameter-types) 1) (+ (count parameter-types) 2))
        (+ (count parameter-types) 1) 
        opening-block-name 
        names'
        (Map.mutable)
    )
)

(const (open-scope builder)
    (ScopeBuilder builder (Env.open-scope (names builder)) (Map.mutable))
)

(const- (names builder)
    (if (ModuleBuilder? builder) 
            (ModuleBuilder-names builder) 
        (FunctionBuilder? builder)
            (FunctionBuilder-names builder)
        (ScopeBuilder-names builder)
    )
)

(const (declare-identified-type! builder name type)
    (include-declaration! builder (Module.IdentifiedType name type))
)

(const (declare-external! builder name return-type parameter-types)
    (include-declaration! builder (Module.External name return-type parameter-types #f))
)

(const (declare-external-global! builder name type align)
    (include-declaration! builder (Module.ExternalGlobal name type align))
)

(const (declare-global! builder name type value align)
    (const builder' (module-builder builder))

    (include-declaration! builder' (Module.Global name type value #f #f #f 8))

    ()
)

(const (declare-string-literal! builder value)
    (const builder' (module-builder builder))

    (const name (str "@.str" (ModuleBuilder-literal-string-count builder')))

    (ModuleBuilder-literal-string-count! builder' (+ 1 (ModuleBuilder-literal-string-count builder')))
    (include-declaration! builder' (Module.Global name (Array (+ (count value) 1) i8) (Operand.string->carray value) #t #t #t 1))

    (Operand.GetElementPtr 
        #t 
        (Type.Pointer Type.i8) 
        (Type.Array (+ (count value) 1) Type.i8) 
        (Operand.GlobalReference name (Type.Pointer (Type.Array (+ (count value) 1) Type.i8)))
        (list (Operand.CInt 64 0) (Operand.CInt 64 0))
    )
)

(const (declare-function! builder function-builder)
    (const builder' (module-builder builder))

    (include-declaration!
        builder'
        (Module.Function 
            (FunctionBuilder-name function-builder) 
            (FunctionBuilder-return-type function-builder) 
            (FunctionBuilder-parameter-types function-builder) 
            (List.reverse (FunctionBuilder-instructions function-builder))
        )
    )
)

(const (build-module builder)
    (Module.Module
        (ModuleBuilder-id builder)
        (List.reverse (ModuleBuilder-declarations builder))
    )
)

(const- (include-declaration! builder declaration)
    (const builder' (module-builder builder))

    (ModuleBuilder-declarations! builder' (pair declaration (ModuleBuilder-declarations builder')))
)

(const (get-name builder name)
    (const env 
        (if (ModuleBuilder? builder) 
                (ModuleBuilder-names builder) 
            (FunctionBuilder? builder)
                (FunctionBuilder-names builder)
            (ScopeBuilder-names builder)
        )
    )

    (Env.get env name)
)

(const (define-name! builder name type)
    (const env 
        (if (ModuleBuilder? builder) 
                (ModuleBuilder-names builder) 
            (FunctionBuilder? builder)
                (FunctionBuilder-names builder)
            (ScopeBuilder-names builder)
        )
    )

    (Env.define-binding! env name type)
)

(const (get-op builder name)
    (if (FunctionBuilder? builder)
            (map-get (FunctionBuilder-ops builder) name)
        (Map.contains? (ScopeBuilder-ops builder) name)
            (map-get (ScopeBuilder-ops builder) name)
        (get-op (ScopeBuilder-builder builder) name)
    )
)

(const (define-op! builder name op)
    (const ops
        (if (FunctionBuilder? builder)
                (FunctionBuilder-ops builder)
            (ScopeBuilder-ops builder)
        )
    )

    (Map.assoc! ops name op)
)

(const (alloca! builder return-type)
    (const builder' (function-builder builder))

    (const var (Operand.LocalReference (new-variable! builder') return-type (FunctionBuilder-block-name builder')))

    (include-instruction! builder' (Instruction.Alloca var 8))

    var
)

(const (br! builder name)
    (const builder' (function-builder builder))

    (include-instruction! builder' (Instruction.Br name))

    ()
)

(const (call-void! builder name arguments)
    (const builder' (function-builder builder))

    (include-instruction! builder' (Instruction.CallVoid name arguments))
)

(const (call! builder name return-type arguments)
    (const builder' (function-builder builder))

    (const var (Operand.LocalReference (new-variable! builder') return-type (FunctionBuilder-block-name builder')))

    (include-instruction! builder' (Instruction.Call var name arguments))

    var
)

(const (cond-br! builder op . labels)
    (const builder' (function-builder builder))

    (include-instruction! builder' (Instruction.CondBr op labels))

    ()
)

(const (icmp! builder op op1 op2)
    (const builder' (function-builder builder))

    (const var (Operand.LocalReference (new-variable! builder') Type.i1 (FunctionBuilder-block-name builder')))

    (include-instruction! builder' (Instruction.ICmp var op op1 op2))

    var
)

(const (label! builder name)
    (const builder' (function-builder builder))

    (include-instruction! builder' (Instruction.Label name))
    (FunctionBuilder-block-name! builder' name)

    ()
)

(const (load! builder operand)
    (const builder' (function-builder builder))

    (const var (Operand.LocalReference (new-variable! builder') (Type.dereference (Operand.type-of operand)) (FunctionBuilder-block-name builder')))

    (include-instruction! builder' (Instruction.Load var operand 8))

    var
)

(const (phi! builder return-type . value-labels)
    (const builder' (function-builder builder))

    (const var (Operand.LocalReference (new-variable! builder') return-type (FunctionBuilder-block-name builder')))

    (include-instruction! builder' (Instruction.Phi var value-labels))

    var
)

(const (ret! builder value)
    (const builder' (function-builder builder))

    (include-instruction! builder' (Instruction.Ret value))
)

(const (store! builder operand address)
    (const builder' (function-builder builder))

    (include-instruction! builder' (Instruction.Store operand address 8))

    ()
)

(const- (include-instruction! builder instruction)
    (const builder' (function-builder builder))

    ;; (println (Instruction.instruction->string instruction))

    (FunctionBuilder-instructions! builder' (pair instruction (FunctionBuilder-instructions builder')))
)

(const- (new-variable! builder)
    (const builder' (function-builder builder))

    (const variable (str "%" (FunctionBuilder-register-count builder')))

    (FunctionBuilder-register-count! builder' (+ (FunctionBuilder-register-count builder') 1))

    variable
)

(const (new-label! builder)
    (const builder' (function-builder builder))

    (const label (FunctionBuilder-label-count builder'))

    (FunctionBuilder-label-count! builder' (+ (FunctionBuilder-label-count builder') 1))

    (str "Block" label)
)

(const (next-const-offset! builder)
    (const builder' (function-builder builder))

    (const r (FunctionBuilder-const-count builder'))
    (FunctionBuilder-const-count! builder' (+ r 1))
    r
)

(const- (module-builder builder)
    (if (FunctionBuilder? builder)
            (module-builder (FunctionBuilder-builder builder))
        (ScopeBuilder? builder)
            (module-builder (ScopeBuilder-builder builder))
        builder
    )
)

(const- (function-builder builder)
    (if (ScopeBuilder? builder)
            (function-builder (ScopeBuilder-builder builder))
        builder
    )
)

(mutable-struct ModuleBuilder
    (id string?)
    (declarations list?)
    (literal-string-count number?)
    (names list?)
)

(mutable-struct FunctionBuilder
    (builder (or? ModuleBuilder? FunctionBuilder?))
    (name string?)
    (return-type Type?)
    (parameter-types (list-of? Type?))
    (instructions list?)
    (label-count integer?)
    (register-count integer?)
    (const-count integer?)
    (block-name string?)
    (names list?)
    (ops map?)
)

(mutable-struct ScopeBuilder
    (builder (or? FunctionBuilder? ScopeBuilder?))

    (names list?)
    (ops map?)
)

(struct Procedure
    (qualified-name string?)
    (frame-level number?)
)

(struct GlobalValue
)

(struct Parameter
    (frame-level number?)
    (frame-offset number?)
)

(struct LocalValue
    (frame-level number?)
    (frame-offset number?)
)

(const (nested-procedure-depth builder)
    (const (runner b)
        (if (ScopeBuilder? b)
                (runner (ScopeBuilder-builder b))
            (FunctionBuilder? b)
                (+ 1 (runner (FunctionBuilder-builder b)))
            0
        )
    )

    (if (ScopeBuilder? builder)
            (nested-procedure-depth (ScopeBuilder-builder builder))
        (FunctionBuilder? builder)
            (runner (FunctionBuilder-builder builder))
        0
    )
)

(const (top-level-procedure? builder)
    (if (ScopeBuilder? builder)
            (top-level-procedure? (ScopeBuilder-builder builder))
        (FunctionBuilder? builder)
            (ModuleBuilder? (FunctionBuilder-builder builder))
        #f
    )
)

(const (nested-procedure-name builder)
    (const (calculate b)
        (if (ScopeBuilder? b)
                (calculate (ScopeBuilder-builder b))
            (FunctionBuilder? b)
                (pair (String.drop (FunctionBuilder-name b) 1) (calculate (FunctionBuilder-builder b)))
            ()
        )
    )

    (str (String.interpolate-with (calculate builder) "_"))
)
