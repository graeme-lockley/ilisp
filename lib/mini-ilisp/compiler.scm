 (import "../list.scm" :as List)
(import "./llvm/builder.scm" :as Builder)
(import "./tst.scm" :as TST)
(import "./llvm/ir/module.scm" :as Module)
(import "./llvm/ir/operand.scm" :as Operand)
(import "./llvm/ir/type.scm" :as Type)

(const- struct-value (Type.Reference "%struct.Value"))
(const- struct-value-pointer (Type.Pointer struct-value))
(const- struct-value-pointer-pointer (Type.Pointer struct-value-pointer))

(const- (zip l1 l2)
    (if (null? l1) ()
        (null? l2) ()
        (pair (list (car l1) (car l2)) (zip (cdr l1) (cdr l2)))
    )
)

(const (tst->ir tst)
    (const builder (Builder.module "test"))

    (Builder.declare-identified-type! builder "%struct.Value" (Type.Structure #f (list Type.i32 (Type.Reference "%union.anon"))))
    (Builder.declare-identified-type! builder "%union.anon" (Type.Structure #f (list (Type.Reference "%struct.NativeClosure"))))
    (Builder.declare-identified-type! builder "%struct.Pair" (Type.Structure #f (list struct-value-pointer struct-value-pointer)))
    (Builder.declare-identified-type! builder "%struct.Vector" (Type.Structure #f (list Type.i32 struct-value-pointer-pointer)))
    (Builder.declare-identified-type! builder "%struct.NativeClosure" (Type.Structure #f (list (Type.Pointer Type.i8) Type.i32 (Type.Pointer Type.i8))))
    (Builder.declare-identified-type! builder "%struct.DynamicClosure" (Type.Structure #f (list (Type.Pointer Type.i8) Type.i32 struct-value-pointer)))

    (Builder.declare-external! builder "@_initialise_lib" Type.void ())
    (Builder.declare-external! builder "@_mk_frame" struct-value-pointer (list struct-value-pointer Type.i32))
    (Builder.declare-external! builder "@_set_frame_value" Type.void (list struct-value-pointer Type.i32 Type.i32 struct-value-pointer))
    (Builder.declare-external! builder "@_get_frame" struct-value-pointer (list struct-value-pointer Type.i32))
    (Builder.declare-external! builder "@_get_frame_value" struct-value-pointer (list struct-value-pointer Type.i32 Type.i32))
    (Builder.declare-external! builder "@_print_value" Type.void (list struct-value-pointer))
    (Builder.declare-external! builder "@_print_newline" Type.void ())
    (Builder.declare-external! builder "@_from_literal_int" struct-value-pointer (list Type.i32))
    (Builder.declare-external! builder "@_from_literal_string" struct-value-pointer (list (Type.Pointer Type.i8)))
    (Builder.declare-external-global! builder "@_VNull" struct-value-pointer 8)
    (Builder.declare-external-global! builder "@_VTrue" struct-value-pointer 8)
    (Builder.declare-external-global! builder "@_VFalse" struct-value-pointer 8)
    (Builder.declare-external! builder "@_plus"  struct-value-pointer (list struct-value-pointer struct-value-pointer))
    (Builder.declare-external! builder "@_minus"  struct-value-pointer (list struct-value-pointer struct-value-pointer))
    (Builder.declare-external! builder "@_multiply"  struct-value-pointer (list struct-value-pointer struct-value-pointer))
    (Builder.declare-external! builder "@_divide"  struct-value-pointer (list struct-value-pointer struct-value-pointer))
    (Builder.declare-external! builder "@_equals"  struct-value-pointer (list struct-value-pointer struct-value-pointer))
    (Builder.declare-external! builder "@_less_than"  struct-value-pointer (list struct-value-pointer struct-value-pointer))
    (Builder.declare-external! builder "@_mk_pair"  struct-value-pointer (list struct-value-pointer struct-value-pointer))
    (Builder.declare-external! builder "@_pair_car"  struct-value-pointer (list struct-value-pointer))
    (Builder.declare-external! builder "@_pair_cdr"  struct-value-pointer (list struct-value-pointer))
    (Builder.declare-external! builder "@_nullp"  struct-value-pointer (list struct-value-pointer))
    (Builder.declare-external! builder "@_booleanp"  struct-value-pointer (list struct-value-pointer))
    (Builder.declare-external! builder "@_integerp"  struct-value-pointer (list struct-value-pointer))
    (Builder.declare-external! builder "@_stringp"  struct-value-pointer (list struct-value-pointer))
    (Builder.declare-external! builder "@_pairp"  struct-value-pointer (list struct-value-pointer))
        
    (for-each (List.filter tst TST.ValueDeclaration?)
        (proc (e)
            (const name (TST.ValueDeclaration-name e))
            (const qualified-name (str "@" name))

            (Builder.declare-global! builder qualified-name struct-value-pointer (Operand.CNull struct-value-pointer) 8)
            (Builder.define-name! builder name (Builder.GlobalValue))
        )
    )

    (for-each (List.filter tst TST.ProcedureDeclaration?)
        (proc (e)
            (const qualified-name (str "@" (TST.ProcedureDeclaration-name e)))
            (const proc-builder (Builder.function builder qualified-name #t struct-value-pointer (List.map (TST.ProcedureDeclaration-arg-names e) (constant struct-value-pointer))))

            (const frame-size (calculate-frame-size e))
            (const v-null-op (compile-expression proc-builder (TST.NullLiteral)))
            (const v-frame-op (Builder.call! proc-builder "@_mk_frame" struct-value-pointer (list v-null-op (Operand.CInt 32 frame-size))))

            (Builder.define-op! proc-builder "%frame" v-frame-op)

            (List.map-idx (TST.ProcedureDeclaration-arg-names e)
                (proc (arg-name idx)
                    (Builder.define-name! proc-builder arg-name (Builder.Parameter (Builder.nested-procedure-depth proc-builder) (+ idx 1)))
                    (Builder.define-op! proc-builder arg-name (Operand.LocalReference (str "%" idx) struct-value-pointer Builder.opening-block-name))

                    (Builder.call-void! proc-builder "@_set_frame_value" (list v-frame-op (Operand.CInt 32 0) (Operand.CInt 32 (+ idx 1)) (Operand.LocalReference (str "%" idx) struct-value-pointer Builder.opening-block-name)))
                )
            )

            (const number-of-arguments (count (TST.ProcedureDeclaration-arg-names e)))
            (List.map-idx (List.filter tst TST.ValueDeclaration?)
                (proc (e idx)
                    (const name (TST.ValueDeclaration-name e))
                    (Builder.define-name! proc-builder name (Builder.LocalValue 0 (+ number-of-arguments idx)))
                )
            )

            (const op (compile-expressions proc-builder (TST.ProcedureDeclaration-es e)))
            (Builder.ret! proc-builder op)

            (Builder.declare-function! builder proc-builder)
        )
    )

    (const main-builder (Builder.function builder "@main" #t Type.i32 ()))
    (Builder.call-void! main-builder "@_initialise_lib" ())

    (for-each tst
        (proc (e)
            (if (TST.CallPrintLn? e)
                    (build-call-print-ln! main-builder e)
                (TST.CallPrint? e)
                    (build-call-print! main-builder e)
                (TST.ValueDeclaration? e)
                    (do (const op (compile-expression main-builder (TST.ValueDeclaration-e e)))
                        (Builder.store! main-builder op (Operand.GlobalReference (str "@" (TST.ValueDeclaration-name e)) struct-value-pointer-pointer))
                    )
                (TST.ProcedureDeclaration? e) ()
                (compile-expression main-builder e)
            )
        )
    )
    (Builder.ret! main-builder (Operand.CInt 32 0))
    (Builder.declare-function! builder main-builder)

    (Builder.build-module builder)
)

(const (calculate-frame-size procedure-decl)
    (+ (count (TST.ProcedureDeclaration-arg-names procedure-decl))
       (count (List.filter (TST.ProcedureDeclaration-es procedure-decl) TST.ValueDeclaration?))
    )
)

(const (compile-expressions builder es)
    (for-each (List.filter es TST.ValueDeclaration?)
        (proc (e)
            (Builder.define-name! 
                builder 
                (TST.ValueDeclaration-name e) 
                (Builder.LocalValue 
                    (Builder.nested-procedure-depth builder) 
                    (Builder.next-const-offset! builder)
                )
            )
        )
    )

    (for-each (List.filter es TST.ProcedureDeclaration?)
        (proc (e)
            (do (const nested-procedure-name (Builder.nested-procedure-name builder))
                (const qualified-name
                    (str 
                        "@" 
                        (if (= "" nested-procedure-name) 
                                (TST.ProcedureDeclaration-name e)
                            (str (TST.ProcedureDeclaration-name e) "_" nested-procedure-name)
                        )
                    )
                )
                
                (const is-top-level-procedure (Builder.ModuleBuilder? builder))

                (const proc-builder 
                    (Builder.function 
                        builder 
                        qualified-name
                        is-top-level-procedure
                        struct-value-pointer 
                        (List.map (TST.ProcedureDeclaration-arg-names e) (constant struct-value-pointer))
                    )
                )
                (Builder.define-name! builder (TST.ProcedureDeclaration-name e) (Builder.Procedure qualified-name (Builder.nested-procedure-depth proc-builder)))

                (const frame-size (calculate-frame-size e))
                (const v-null-op (compile-expression proc-builder (TST.NullLiteral)))
                (const v-frame-op (Builder.call! proc-builder "@_mk_frame" struct-value-pointer (list v-null-op (Operand.CInt 32 frame-size))))

                (Builder.define-op! proc-builder "%frame" v-frame-op)

                (List.map-idx (TST.ProcedureDeclaration-arg-names e)
                    (proc (arg-name idx)
                        (const idx'
                            (if is-top-level-procedure 
                                idx 
                                (+ 1 idx)
                            )
                        )

                        (Builder.define-name! proc-builder arg-name (Builder.Parameter (Builder.nested-procedure-depth proc-builder) idx'))
                        (const op (Operand.LocalReference (str "%" idx') struct-value-pointer Builder.opening-block-name))
                        (Builder.define-op! proc-builder arg-name op)

                        (Builder.call-void! proc-builder "@_set_frame_value" (list v-frame-op (Operand.CInt 32 0) (Operand.CInt 32 (+ idx 1)) op))
                    )
                )

                (const es (TST.ProcedureDeclaration-es e))

                (const number-of-arguments (count (TST.ProcedureDeclaration-arg-names e)))
                (List.map-idx (List.filter es TST.ValueDeclaration?)
                    (proc (e idx)
                        (const name (TST.ValueDeclaration-name e))
                        (Builder.define-name! proc-builder name (Builder.LocalValue 0 (+ number-of-arguments idx)))
                    )
                )

                (const op (compile-expressions proc-builder es))
                (Builder.ret! proc-builder op)

                (Builder.declare-function! builder proc-builder)
            )
        )
    )

    (const op (fold es () (proc (op e) (compile-expression builder e))))

    (if (null? op) (compile-expression builder (TST.NullLiteral)) op)
)

(const (compile-expression builder e)
    (if (TST.NullLiteral? e)
            (Builder.load! builder (Operand.GlobalReference "@_VNull" struct-value-pointer-pointer))
        (TST.StringLiteral? e)
            (do (const op (Builder.declare-string-literal! builder (TST.StringLiteral-value e)))
                (Builder.call! builder "@_from_literal_string" struct-value-pointer (list op))
            )
        (TST.IntegerLiteral? e)
            (Builder.call! builder "@_from_literal_int" struct-value-pointer (list (Operand.CInt 32 (TST.IntegerLiteral-value e))))
        (TST.BooleanLiteral? e)
            (do (const name (if (TST.BooleanLiteral-value e) "@_VTrue" "@_VFalse"))
                (Builder.load! builder (Operand.GlobalReference name struct-value-pointer-pointer))
            )
        (TST.BinaryOperator? e)
            (do (const op (TST.BinaryOperator-op e))
                (const op1 (compile-expression builder (TST.BinaryOperator-op1 e)))
                (const op2 (compile-expression builder (TST.BinaryOperator-op2 e)))

                (Builder.call! builder (map-get binary-procedure-names op) struct-value-pointer (list op1 op2))
            )
        (TST.IfThenElse? e)
            (do (const e1 (compile-expression builder (TST.IfThenElse-e1 e)))
                (const false-value (Builder.load! builder (Operand.GlobalReference "@_VFalse" struct-value-pointer-pointer)))

                (const e1-compare (Builder.icmp! builder 'ne e1 false-value))

                (const then-label (Builder.new-label! builder))
                (const else-label (Builder.new-label! builder))
                (const merge-label (Builder.new-label! builder))

                (Builder.cond-br! builder e1-compare then-label else-label)
            
                (Builder.label! builder then-label)
                (const e2 (compile-expression builder (TST.IfThenElse-e2 e)))                
                (Builder.br! builder merge-label)
            
                (Builder.label! builder else-label)
                (const e3 (compile-expression builder (TST.IfThenElse-e3 e)))
                (Builder.br! builder merge-label)

                (Builder.label! builder merge-label)
                (Builder.phi! builder struct-value-pointer e2 e3)
            )
        (TST.Pair? e)
            (do (const e1 (compile-expression builder (TST.Pair-car e)))
                (const e2 (compile-expression builder (TST.Pair-cdr e)))
                (Builder.call! builder "@_mk_pair" struct-value-pointer (list e1 e2))
            )
        (TST.Car? e)
            (do (const e' (compile-expression builder (TST.Car-e e)))
                (Builder.call! builder "@_pair_car" struct-value-pointer (list e'))
            )
        (TST.Cdr? e)
            (do (const e' (compile-expression builder (TST.Cdr-e e)))
                (Builder.call! builder "@_pair_cdr" struct-value-pointer (list e'))
            )
        (TST.NullP? e)
            (do (const e' (compile-expression builder (TST.NullP-e e)))
                (Builder.call! builder "@_nullp" struct-value-pointer (list e'))
            )
        (TST.BooleanP? e)
            (do (const e' (compile-expression builder (TST.BooleanP-e e)))
                (Builder.call! builder "@_booleanp" struct-value-pointer (list e'))
            )
        (TST.IntegerP? e)
            (do (const e' (compile-expression builder (TST.IntegerP-e e)))
                (Builder.call! builder "@_integerp" struct-value-pointer (list e'))
            )
        (TST.StringP? e)
            (do (const e' (compile-expression builder (TST.StringP-e e)))
                (Builder.call! builder "@_stringp" struct-value-pointer (list e'))
            )
        (TST.PairP? e)
            (do (const e' (compile-expression builder (TST.PairP-e e)))
                (Builder.call! builder "@_pairp" struct-value-pointer (list e'))
            )
        (TST.IdentifierReference? e)
            (do (const identifier-name (TST.IdentifierReference-name e))
                (const name (Builder.get-name builder identifier-name))

                ;; (if (= identifier-name "a")
                ;;         (do (println "Name: " identifier-name)
                ;;             (println (pr-str name))
                ;;             (println (pr-str (Builder.FunctionBuilder-names builder)))
                ;;         )
                ;; )

                (if (null? name)
                        (compile-expression builder (TST.NullLiteral))
                    (Builder.Parameter? name)
                        (if (Builder.op? builder identifier-name)
                                (Builder.get-op builder identifier-name)
                            (do (const op (Builder.call! builder "@_get_frame_value" struct-value-pointer 
                                        (list 
                                            (Operand.LocalReference "%0" struct-value-pointer Builder.opening-block-name)
                                            (Operand.CInt 32 (- (Builder.nested-procedure-depth builder) (Builder.Parameter-frame-level name) 1))
                                            (Operand.CInt 32 (Builder.Parameter-frame-offset name))
                                        )
                                    )
                                )
                                (Builder.define-op! builder identifier-name op)

                                op
                            )
                        )
                    (Builder.LocalValue? name)
                        (if (Builder.op? builder identifier-name)
                                (Builder.get-op builder identifier-name)
                            (= (Builder.LocalValue-frame-level name) (Builder.nested-procedure-depth builder))
                                (compile-expression builder (TST.NullLiteral))
                            (do (const op (Builder.call! builder "@_get_frame_value" struct-value-pointer 
                                        (list 
                                            (Operand.LocalReference "%0" struct-value-pointer Builder.opening-block-name)
                                            (Operand.CInt 32 (- (Builder.nested-procedure-depth builder) (Builder.LocalValue-frame-level name) 1))
                                            (Operand.CInt 32 (Builder.LocalValue-frame-offset name))
                                        )
                                    )
                                )
                                (Builder.define-op! builder identifier-name op)

                                op
                            )
                        )
                    (do (const qualified-name (str "@" identifier-name))
                        (const op (Operand.GlobalReference qualified-name struct-value-pointer-pointer))
                        (Builder.load! builder op)
                    )
                )
            )
        (TST.CallProcedure? e)
            (do (const procedure-name (TST.CallProcedure-name e))
                (const name (Builder.get-name builder procedure-name))

                (const qualified-name
                    (if (null? name) (str "@" procedure-name)
                        (Builder.Procedure-qualified-name name)
                    )
                )

                (const call-level
                    (if (null? name) 0
                        (Builder.Procedure-frame-level name)
                    )
                )
                
                (const ops (List.map (TST.CallProcedure-arguments e) (proc (e') (compile-expression builder e'))))

                (const ops'
                    (if (= call-level 0)
                            ops
                        (do (const level (Builder.nested-procedure-depth builder))

                            (if (= level call-level)
                                    (pair (Operand.LocalReference "%0" struct-value-pointer Builder.opening-block-name) ops)
                                (> call-level level)
                                    (pair (Builder.get-op builder "%frame") ops)
                                (do (const op (Builder.call! builder "@_get_frame" struct-value-pointer (list (Builder.get-op builder "%frame") (Operand.CInt 32 (- level call-level)))))
                                    (pair op ops)
                                )
                            )
                        )
                    )
                )

                (Builder.call! builder qualified-name struct-value-pointer ops')
            )
        (TST.CallPrint? e)
            (do (build-call-print! builder e)
                (compile-expression builder (TST.NullLiteral))
            )
        (TST.CallPrintLn? e)
            (do (build-call-print-ln! builder e)
                (compile-expression builder (TST.NullLiteral))
            )
        (TST.ValueDeclaration? e)
            (do (const op (compile-expression builder (TST.ValueDeclaration-e e)))

                (const name (Builder.get-name builder (TST.ValueDeclaration-name e)))
                (Builder.define-op! builder (TST.ValueDeclaration-name e) op)

                (if (Builder.LocalValue? name)
                        (Builder.call-void! builder "@_set_frame_value"
                            (list
                                ;; (Operand.LocalReference "%0" struct-value-pointer Builder.opening-block-name)
                                (Builder.get-op builder "%frame")
                                (Operand.CInt 32 0)
                                (Operand.CInt 32 (Builder.LocalValue-frame-offset name))
                                op
                            )
                        )
                    (println "Internal Error: Excepted LocalValue: " (pr-str name))
                )

                op
            )
        (TST.ProcedureDeclaration? e)
            (compile-expression builder (TST.NullLiteral))
        (raise 'TODO-compile e)
    )
)

(const binary-procedure-names
    {   '+ "@_plus" 
        '- "@_minus"
        '* "@_multiply"
        '/ "@_divide"
        '= "@_equals"
        '< "@_less_than"
    }
)

(const (build-call-print! builder e)
    (for-each (TST.CallPrint-args e)
        (proc (arg)
            (const operand (compile-expression builder arg))
            (Builder.call-void! builder "@_print_value" (list operand))
        )
    )
)

(const (build-call-print-ln! builder e)
    (for-each (TST.CallPrintLn-args e)
        (proc (arg)
            (const operand (compile-expression builder arg))
            (Builder.call-void! builder "@_print_value" (list operand))
        )
    )
    (Builder.call-void! builder "@_print_newline" ())
)

(const (assemble ll-file bc-file)
    (*builtin*.exec (str "llvm-as-10 " ll-file " -o " bc-file))
)

(const (link bc-file ilib-file-name binary-file)
    (*builtin*.exec (str "clang " bc-file " " ilib-file-name " -o " binary-file " 2>&1"))
)

(const (run bc-file)
    (*builtin*.exec (str "clang " bc-file " ./scratch/lib.bc -o a.out 2>&1"))
    (*builtin*.exec "./a.out")
)
