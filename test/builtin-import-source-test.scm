(import "../lib/unit.scm" :as Unit)

(Unit.test "healthy import"
    (const healthy-import-file-name (*builtin*.file-name-relative-to-file-name *source-name* "./builtin-import-source-test/healthy-import.scm"))

    (Unit.assert-falsy (contains? *imports* healthy-import-file-name))

    (const HealthyImport (*builtin*.import-source "./builtin-import-source-test/healthy-import.scm"))
    (Unit.assert-truthy (contains? *imports* healthy-import-file-name))

    (Unit.assert-equals (HealthyImport.inc 10) 11)
    (Unit.assert-equals HealthyImport.*source-name* healthy-import-file-name)
)

(Unit.test "cyclic dependency"
    (Unit.assert-signal-name (*builtin*.import-source "./builtin-import-source-test/cycle-A.scm") 'CyclicModuleDependency)
)
