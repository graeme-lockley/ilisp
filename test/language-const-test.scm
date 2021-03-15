(import "../lib/unit.scm" :as Unit)

(const *builtin* (get (car **root**) :builtins))

(Unit.test "const atom"
    (const s (*builtin*.atom 0))
    (Unit.assert-truthy (*builtin*.atom? s))
)

(Unit.test "const procedure"
    (const (inc n) (+ n 1))

    (Unit.assert-equals (inc (inc (inc 2))) 5)
)

(Unit.test "const import procedure"
    (import "./language-const-import.scm" :as Import)

    (Unit.assert-equals (Import.inc (Import.inc (Import.inc 2))) 5)
    (Unit.assert-signal-name (Import.plus 2 3) 'UnknownSymbolInPackage)
)
