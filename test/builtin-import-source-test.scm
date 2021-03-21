(import "../lib/unit.scm" :as Unit)

(Unit.test "get source name"
    (println (*builtin*.import-source "./language-const-import.scm"))

    (Unit.assert-equals ((get (*builtin*.import-source "./language-const-import.scm") 'inc) 10) 11)
)
