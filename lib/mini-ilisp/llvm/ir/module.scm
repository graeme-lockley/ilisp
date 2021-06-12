(import "../../../list.scm" :as List)
(import "../../../data/struct.scm" :names struct)
(import "../../../data/union.scm" :names union)
(import "../../../predicate.scm" :names =? boolean? byte-vector? character? integer? list? list-of? null? string?)
(import "../../../string.scm" :as String)

(import "./instruction.scm" :as Instruction)
(import "./operand.scm" :as Operand)
(import "./type.scm" :names Type? type->string)

(struct IdentifiedType
    (name string?)
    (type Type?)
)

(struct External
    (name string?)
    (return-type Type?)
    (parameter-types (list-of? Type?))
    (is-var-arg boolean?)
)

(struct ExternalGlobal
    (name string?)
    (type Type?)
    (align number?)
)

(struct Global
    (name string?)
    (type Type?)
    (value Operand.Constant?)

    (private boolean?)
    (unnamed-addr boolean?)
    (constant boolean?)
    (align number?)
)

(struct Function
    (name string?)
    (return-type Type?)
    (parameter-types (list-of? Type?))
    (instructions (list-of? Instruction.Instruction?))
)

(union Declaration
    IdentifiedType? External? ExternalGlobal? Global? Function?
)

(struct Module
    (name string?)
    (declarations (list-of? Declaration))
)

(const (identified-type->string identified-type)
    (str (IdentifiedType-name identified-type) " = type " (type->string (IdentifiedType-type identified-type)))
)

(const- (riap l e)
    (if (null? l) (list e)
        (pair (car l) (riap (cdr l) e))
    )
)

(const (external->string external)
    (const parameters
        (if (External-is-var-arg external)
                (String.interpolate-with (riap (List.map (External-parameter-types external) type->string) "...") ", ")
            (String.interpolate-with (List.map (External-parameter-types external) type->string) ", ")            
        )
    )

    (str "declare " (type->string (External-return-type external)) " " (External-name external) "(" parameters ")")
)

(const (external-global->string external)
    (str 
        (ExternalGlobal-name external) 
        " = external global " 
        (type->string (ExternalGlobal-type external)) 
        (if (= 0 (ExternalGlobal-align external)) "" (str ", align " (ExternalGlobal-align external)))
    )
)

(const (global->string global)
    (str
        (Global-name global)
        " = "
        (if (Global-private global) "private " "")
        (if (Global-unnamed-addr global) "unnamed_addr " "")
        (if (Global-constant global) "constant " "")
        (type->string (Global-type global))
        " "
        (Operand.untyped-operand->string (Global-value global))
        (if (= 0 (Global-align global)) "" (str ", align " (Global-align global)))
    )
)

(const (function->string function)
    (str
        "define "
        (type->string (Function-return-type function))
        " "
        (Function-name function)
        "("
        (String.interpolate-with (List.map-idx (Function-parameter-types function) (proc (t i) (str (type->string t) " %" i))) ", ")
        ") {\n"
        (String.interpolate-with (List.map (Function-instructions function) (proc (i) (str "  " (Instruction.instruction->string i) "\n"))) "")
        "}\n"
    )
)

(const (declaration->string declaration)
    (if (IdentifiedType? declaration)
            (identified-type->string declaration)
        (External? declaration)
            (external->string declaration)
        (ExternalGlobal? declaration)
            (external-global->string declaration)
        (Global? declaration)
            (global->string declaration)
        (Function? declaration)
            (function->string declaration)
    )
)

(const (module->string module)
    (str
        "; ModuleID = '" (Module-name module) "'\n"
        "\n"
        (String.interpolate-with (List.map (Module-declarations module) declaration->string) "\n\n")
    )
)