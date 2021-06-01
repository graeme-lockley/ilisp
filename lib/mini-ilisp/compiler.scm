(import "./llvm/builder.scm" :as Builder)
(import "./tst.scm" :as TST)
(import "./llvm/ir/type.scm" :as Type)

(const (tst->ir tst)
    (const builder (Builder.mk "test"))

    (Builder.declare-identified-type builder "%struct.Value" (Type.Structure #f (list Type.i32 (Type.Reference "%union.anon"))))
    (Builder.declare-identified-type builder "%union.anon" (Type.Structure #f (list (Type.Pointer Type.i32))))

    ;; (Builder.declare-external "_print_value" )
)

(const (assemble ll-file bc-file)
    ()
)

(const (run bc-file)
    "hello world\n"
)
