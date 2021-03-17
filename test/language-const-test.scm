(import "../lib/unit.scm" :as Unit)

(Unit.test "const atom"
    (const- s (*builtin*.atom 0))
    (Unit.assert-truthy (*builtin*.atom? s))
)

(Unit.test "const procedure"
    (const- (inc n) (+ n 1))

    (Unit.assert-equals (inc (inc (inc 2))) 5)
)

(Unit.test "const import procedure"
    (import "./language-const-import.scm" :as Import)

    (Unit.assert-equals (Import.inc (Import.inc (Import.inc 2))) 5)
    (Unit.assert-signal-name (Import.plus 2 3) 'UnknownSymbolInPackage)
)

(Unit.test "let variable"
    (let- s 0)

    (Unit.assert-truthy (*builtin*.atom? s))
    (Unit.assert-equals (*builtin*.atom-dereference s) 0)
)

(Unit.test "const procedure"
    (let- (inc n) (+ n 1))

    (Unit.assert-equals ((*builtin*.atom-dereference inc) ((*builtin*.atom-dereference inc) ((*builtin*.atom-dereference inc) 2))) 5)
)

(Unit.test "const import procedure"
    (import "./language-const-import.scm" :as Import)

    (Unit.assert-equals ((*builtin*.atom-dereference Import.dec) 2) 1)
    (Unit.assert-signal-name (Import.minus 2 3) 'UnknownSymbolInPackage)
)

