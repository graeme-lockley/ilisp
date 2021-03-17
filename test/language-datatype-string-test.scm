(import "../lib/unit.scm" :as Unit)

(Unit.test "reader"
    (Unit.assert-equals (*builtin*.string-nth "\t" 0) #x09)
    (Unit.assert-equals (*builtin*.string-nth "\n" 0) #x0a)
    (Unit.assert-equals (*builtin*.string-nth "\r" 0) #x0d)
    (Unit.assert-equals (*builtin*.string-nth "\\" 0) #\\)
    (Unit.assert-equals (*builtin*.string-nth "\"" 0) #x22)
    (Unit.assert-equals (*builtin*.string-nth "\x22;" 0) #x22)
)
