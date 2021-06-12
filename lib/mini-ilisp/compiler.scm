(import "./llvm/builder.scm" :as Builder)
(import "./tst.scm" :as TST)
(import "./llvm/ir/module.scm" :as Module)
(import "./llvm/ir/operand.scm" :as Operand)
(import "./llvm/ir/type.scm" :as Type)

(const- struct-value (Type.Reference "%struct.Value"))
(const- struct-value-pointer (Type.Pointer struct-value))
(const- struct-value-pointer-pointer (Type.Pointer struct-value-pointer))

(const (tst->ir tst)
    (const builder (Builder.module "test"))

    (Builder.declare-identified-type! builder "%struct.Value" (Type.Structure #f (list Type.i32 (Type.Reference "%union.anon"))))
    (Builder.declare-identified-type! builder "%union.anon" (Type.Structure #f (list (Type.Pointer Type.i32))))

    (Builder.declare-external! builder "@_initialise_lib" Type.void ())
    (Builder.declare-external! builder "@_print_value" Type.void (list struct-value-pointer))
    (Builder.declare-external! builder "@_print_newline" Type.void ())
    (Builder.declare-external! builder "@_from_literal_int" struct-value-pointer (list Type.i32))
    (Builder.declare-external! builder "@_from_literal_string" struct-value-pointer (list (Type.Pointer Type.i8)))
    (Builder.declare-external-global! builder "@_VTrue" struct-value-pointer 8)
    (Builder.declare-external-global! builder "@_VFalse" struct-value-pointer 8)
        
    (const main-builder (Builder.function builder "@main" Type.i32 ()))

    (Builder.call-void! main-builder "@_initialise_lib" ())

    (for-each tst 
        (proc (e)
            (if (TST.CallPrintLn? e)
                    (build-call-print-ln! main-builder e)
                (TST.CallPrint? e)
                    (build-call-print! main-builder e)
                (raise 'TODO-compile e)
            )
        )
    )
    (Builder.ret! main-builder (Operand.CInt 32 0))
    (Builder.declare-function! builder main-builder)

    (Builder.build-module builder)
)

(const (compile-expression builder e)
    (if (TST.StringLiteral? e)
            (do (const op (Builder.declare-string-literal! builder (TST.StringLiteral-value e)))
                (Builder.call! builder "@_from_literal_string" struct-value-pointer (list op))
            )
        (TST.IntegerLiteral? e)
            (Builder.call! builder "@_from_literal_int" struct-value-pointer (list (Operand.CInt 32 (TST.IntegerLiteral-value e))))
        (TST.BooleanLiteral? e)
            (do (const name (if (TST.BooleanLiteral-value e) "@_VTrue" "@_VFalse"))
                (Builder.load! builder (Operand.LocalReference name struct-value-pointer-pointer))
            )
        (TST.CallPrintLn? e)
            (build-call-print-ln! main-builder e)
        (raise 'TODO-compile e)
    )
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

(const (run bc-file)
    (*builtin*.exec (str "clang " bc-file " ./scratch/lib.bc -o a.out 2>&1"))
    (*builtin*.exec "./a.out")
)
