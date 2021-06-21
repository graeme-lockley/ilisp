(import "../../data/struct.scm" :names mutable-struct struct)
(import "../../predicate.scm" :names =? any? byte-vector? character? integer? list? list-of? map? null? string?)
(import "../../list.scm" :as List)
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

(const (function builder name return-type parameter-types)
    (if (ModuleBuilder? builder)
            (do (const names (Env.open-scope (ModuleBuilder-names builder)))
                (const bindings (Env.empty))

                (FunctionBuilder builder name return-type parameter-types () 1 (+ (count parameter-types) 1) opening-block-name names bindings)
            )
        (do (const names (Env.open-scope (FunctionBuilder-names builder)))
            (const bindings (Env.empty))

            (FunctionBuilder builder name return-type parameter-types () 1 (+ (count parameter-types) 1) opening-block-name names bindings)
        )
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
    (const builder' (if (FunctionBuilder? builder) (FunctionBuilder-module-builder builder) builder))

    (include-declaration! builder' (Module.Global name type value #f #f #f 8))

    ()
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
        (Operand.GlobalReference name (Type.Pointer (Type.Array (+ (count value) 1) Type.i8)))
        (list (Operand.CInt 64 0) (Operand.CInt 64 0))
    )
)

(const (declare-function! builder function-builder)
    (if (ModuleBuilder? builder)
            (include-declaration!
                builder
                (Module.Function 
                    (FunctionBuilder-name function-builder) 
                    (FunctionBuilder-return-type function-builder) 
                    (FunctionBuilder-parameter-types function-builder) 
                    (List.reverse (FunctionBuilder-instructions function-builder))
                )
            )
        (declare-function! (FunctionBuilder-module-builder builder) function-builder)
    )
)

(const (build-module builder)
    (Module.Module
        (ModuleBuilder-id builder)
        (List.reverse (ModuleBuilder-declarations builder))
    )
)

(const- (include-declaration! builder declaration)
    (ModuleBuilder-declarations! builder (pair declaration (ModuleBuilder-declarations builder)))
)

(const (get-name builder name)
    (const env (ModuleBuilder? builder) (ModuleBuilder-names builder) (FunctionBuilder-names builder))

    (Env.get env name)
)

(const (define-name! builder name type)
    (const env (if (ModuleBuilder? builder) (ModuleBuilder-names builder) (FunctionBuilder-names builder)))

    (Env.define-binding! env name type)
)

(const (get-binding builder name)
    (Env.get (FunctionBuilder-bindings builder) name)
)

(const (define-binding! builder name op)
    (Env.define-binding! (FunctionBuilder-bindings builder) name op)
)

(const (alloca! builder return-type)
    (const var (Operand.LocalReference (new-variable! builder) return-type (FunctionBuilder-block-name builder)))

    (include-instruction! builder (Instruction.Alloca var 8))

    var
)

(const (br! builder name)
    (include-instruction! builder (Instruction.Br name))

    ()
)

(const (call-void! builder name arguments)
    (include-instruction! builder (Instruction.CallVoid name arguments))
)

(const (call! builder name return-type arguments)
    (const var (Operand.LocalReference (new-variable! builder) return-type (FunctionBuilder-block-name builder)))

    (include-instruction! builder (Instruction.Call var name arguments))

    var
)

(const (cond-br! builder op . labels)
    (include-instruction! builder (Instruction.CondBr op labels))

    ()
)

(const (icmp! builder op op1 op2)
    (const var (Operand.LocalReference (new-variable! builder) Type.i1 (FunctionBuilder-block-name builder)))

    (include-instruction! builder (Instruction.ICmp var op op1 op2))

    var
)

(const (label! builder name)
    (include-instruction! builder (Instruction.Label name))
    (FunctionBuilder-block-name! builder name)

    ()
)

(const (load! builder operand)
    (const var (Operand.LocalReference (new-variable! builder) (Type.dereference (Operand.type-of operand)) (FunctionBuilder-block-name builder)))

    (include-instruction! builder (Instruction.Load var operand 8))

    var
)

(const (phi! builder return-type . value-labels)
    (const var (Operand.LocalReference (new-variable! builder) return-type (FunctionBuilder-block-name builder)))

    (include-instruction! builder (Instruction.Phi var value-labels))

    var
)

(const (ret! builder value)
    (include-instruction! builder (Instruction.Ret value))
)

(const (store! builder operand address)
    (include-instruction! builder (Instruction.Store operand address 8))

    ()
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

(const (new-label! builder)
    (const label (FunctionBuilder-label-count builder))

    (FunctionBuilder-label-count! builder (+ (FunctionBuilder-label-count builder) 1))

    (str "Block" label)
)

(mutable-struct ModuleBuilder
    (id string?)
    (declarations list?)
    (literal-string-count number?)
    (names list?)
)

(mutable-struct FunctionBuilder
    (module-builder (or? ModuleBuilder? FunctionBuilder?))
    (name string?)
    (return-type Type?)
    (parameter-types (list-of? Type?))
    (instructions list?)
    (label-count integer?)
    (register-count integer?)
    (block-name string?)
    (names list?)
    (bindings (list-of? map?))
)

(struct Procedure
    (qualified-name string?)
)

(struct GlobalValue
)

(struct LocalValue
)

(const (nested-procedure-name builder)
    (const (calculate b)
        (if (FunctionBuilder? b)
                (pair (String.drop (FunctionBuilder-name b) 1) (calculate (FunctionBuilder-module-builder b)))
            ()
        )
    )

    (str (String.interpolate-with (calculate builder) "_"))
)