(import "../lib/unit.scm" :as Unit)

(define byte-vector (get (car **root**) :builtins 'byte-vector))
(define byte-vector-count (get (car **root**) :builtins 'byte-vector-count))

(Unit.test "count"
    (Unit.assert-equals (byte-vector-count (byte-vector)) 0)
    (Unit.assert-equals (byte-vector-count (byte-vector 1)) 1)
    (Unit.assert-equals (byte-vector-count (byte-vector 1 2 3)) 3)
)
