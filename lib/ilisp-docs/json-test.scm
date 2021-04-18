(import "../unit.scm" :as Unit)
(import "./json.scm" :as JSON)

(Unit.test "->string - string value"
    (Unit.assert-equals (JSON.->string "hello") "\"hello\"")
    (Unit.assert-equals (JSON.->string "he\"llo") "\"he\\\"llo\"")
    (Unit.assert-equals (JSON.->string "he\tllo") "\"he\\tllo\"")

    (Unit.assert-equals (JSON.->string "he\nllo") "\"he\\nllo\"")
)

(Unit.test "->string - symbol value"
    (Unit.assert-equals (JSON.->string 'hello) "\"hello\"")
)

(Unit.test "->string - keyword value"
    (Unit.assert-equals (JSON.->string :hello) "\":hello\"")
)

(Unit.test "->string - integer value"
    (Unit.assert-equals (JSON.->string 123) "123")
)

(Unit.test "->string - boolean value"
    (Unit.assert-equals (JSON.->string #t) "true")
    (Unit.assert-equals (JSON.->string #f) "false")
)

(Unit.test "->string - list value"
    (Unit.assert-equals (JSON.->string '()) "[]")
    (Unit.assert-equals (JSON.->string '(1)) "[1]")
    (Unit.assert-equals (JSON.->string '(1 2 3)) "[1, 2, 3]")
)

(Unit.test "->string - pair but not list value"
    (Unit.assert-equals (JSON.->string (pair 1 2)) "[1, 2]")
)

(Unit.test "->string - vector value"
    (Unit.assert-equals (JSON.->string []) "[]")
    (Unit.assert-equals (JSON.->string [1]) "[1]")
    (Unit.assert-equals (JSON.->string [1 2 3]) "[1, 2, 3]")
)

(Unit.test "->string - map value"
    (Unit.assert-equals (JSON.->string {}) "{}")
    (Unit.assert-equals (JSON.->string {'a 10 'b "hello"}) "{\"a\": 10, \"b\": \"hello\"}");
    (Unit.assert-equals (JSON.->string {'a 10 'b (list "hello" "world") 'c {'x #f 'y #t}}) "{\"a\": 10, \"b\": [\"hello\", \"world\"], \"c\": {\"x\": false, \"y\": true}}");
)
