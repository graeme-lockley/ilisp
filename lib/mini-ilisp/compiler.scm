(import "./llvm/builder.scm" :as Builder)
(import "./tst.scm" :as TST)
(import "./llvm/ir/module.scm" :as Module)
(import "./llvm/ir/operand.scm" :as Operand)
(import "./llvm/ir/type.scm" :as Type)

(const (tst->ir tst)
    (const builder (Builder.module "test"))

    (Builder.declare-identified-type! builder "%struct.Value" (Type.Structure #f (list Type.i32 (Type.Reference "%union.anon"))))
    (Builder.declare-identified-type! builder "%union.anon" (Type.Structure #f (list (Type.Pointer Type.i32))))

    (Builder.declare-external! builder "@_print_value" Type.void (list (Type.Pointer (Type.Reference "%struct.Value"))))
    (Builder.declare-external! builder "@_print_newline" Type.void ())
    (Builder.declare-external! builder "@_from_literal_string" (Type.Pointer (Type.Reference "%struct.Value")) (list (Type.Pointer Type.i8)))
        
    (const main-builder (Builder.function builder "@main" Type.i32 ()))

    (for-each tst (proc (e)
        (if (TST.CallPrintLn? e)
                (build-call-print-ln! main-builder e)
            (raise TODO-compile e)
        )
    ))
    (Builder.ret! main-builder (Operand.CInt 32 0))
    (Builder.declare-function! builder main-builder)

    (Builder.build-module builder)
)

(const (compile-expression builder e)
    (if (TST.StringLiteral? e)
            (do (const op (Builder.declare-string-literal! builder (TST.StringLiteral-value e)))
                (Builder.call! builder "@_from_literal_string" (Type.Pointer (Type.Reference "%struct.Value")) (list op))
            )
        (TST.CallPrintLn? e)
            (build-call-print-ln! main-builder e)
        (raise 'TODO-compile e)
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
    ()
)

(const (run bc-file)
    "hello world\n"
)
