(import "../lib/unit.scm" :as Unit)

(define string-nth (get (car **root**) :builtins 'string-nth))

(Unit.test "reader"
    (Unit.assert-equals (string-nth "\t" 0) #x09)
    (Unit.assert-equals (string-nth "\n" 0) #x0a)
    (Unit.assert-equals (string-nth "\r" 0) #x0d)
    (Unit.assert-equals (string-nth "\\" 0) #\\)
    (Unit.assert-equals (string-nth "\"" 0) #x22)
    (Unit.assert-equals (string-nth "\x22;" 0) #x22)
)
