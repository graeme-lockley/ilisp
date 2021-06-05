(import "./llvm/builder.scm" :as Builder)
(import "./tst.scm" :as TST)
(import "./llvm/ir/type.scm" :as Type)

(const (tst->ir tst)
    (const builder (Builder.module "test"))

    (Builder.declare-identified-type! builder "%struct.Value" (Type.Structure #f (list Type.i32 (Type.Reference "%union.anon"))))
    (Builder.declare-identified-type! builder "%union.anon" (Type.Structure #f (list (Type.Pointer Type.i32))))

    (Builder.declare-external! builder "@_print_value" Type.void (list (Type.Pointer (Type.Reference "%struct.Value"))))
        
    (const main-builder (Builder.function builder "@main" Type.i32 ()))

    (for-each tst (proc (e)
        (if (TST.CallPrintLn? e)
                (build-call-print-ln! main-builder e)
            (raise TODO-compile e)
        )
    ))

    ;; (Builder.build-function! main-builder)
    ;; (const main-builder (Builder.declare-function builder "@main" Type.i32 ()))

    ;; (Builder.build-module builder)

    ()
)

(const (compile-expression builder e)
    (if (TST.CallPrintLn? e)
            (build-call-print-ln! main-builder e)
        (raise TODO-compile e)
    )
)

(const (build-call-print-ln! builder e)
    ;; (raise TODO-build-call-print-ln! e)
    ()
)

(const (assemble ll-file bc-file)
    ()
)

(const (run bc-file)
    "hello world\n"
)
