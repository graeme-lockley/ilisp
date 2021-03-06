(import "../unit.scm" :as Unit)
(import "./reader.scm" :as Reader)

(Unit.test "drop-right-while"
    (Unit.assert-equals 
        (Reader.drop-right-while '("hello" "") (proc (l) (= l ""))) 
        '("hello")
    )
)

(Unit.test "parse-propery-description"
    (Unit.assert-equals 
        (Reader.parse-property-description ()) 
        (pair () ())
    )
    
    (Unit.assert-equals 
        (Reader.parse-property-description '(":hello" "world")) 
        (pair () '(":hello" "world"))
    )
    
    (Unit.assert-equals 
        (Reader.parse-property-description '("hello" "world")) 
        (pair '("hello" "world") ())
    )
    
    (Unit.assert-equals 
        (Reader.parse-property-description '("hello" "world" ":signal SystemError")) 
        (pair '("hello" "world") '(":signal SystemError"))
    )
)

(Unit.test "parse-propery-comments - usage"
    (Unit.assert-equals 
        (Reader.parse-property-comments '(":usage (socket-write-all socket bytes)" "hello" "world")) 
        (pair (pair {'type "usage" 'signature "(socket-write-all socket bytes)" 'description '("hello" "world")} ()) ())
    )
)

(Unit.test "parse-propery-comments - parameter"
    (Unit.assert-equals 
        (Reader.parse-property-comments '(":parameter bytes byte-vector?" "hello" "world")) 
        (pair (pair {'type "parameter" 'name "bytes" 'signature "byte-vector?" 'description '("hello" "world")} ()) ())
    )
)

(Unit.test "parse-propery-comments - returns"
    (Unit.assert-equals 
        (Reader.parse-property-comments '(":returns byte-vector?" "hello" "world")) 
        (pair (pair {'type "returns" 'signature "byte-vector?" 'description '("hello" "world")} ()) ())
    )
)

(Unit.test "parse-propery-comments - signal"
    (Unit.assert-equals 
        (Reader.parse-property-comments '(":signal SystemError" "hello" "world")) 
        (pair (pair {'type "signal" 'signature "SystemError" 'description '("hello" "world")} ()) ())
    )
)

(Unit.test "parse - no document"
    (Unit.assert-equals (Reader.parse "(const thread? *builtin*.thread?)\n")
        (list 
            {   'description () 
                'name "thread?" 
                'properties ()
            }
        )
    )

    (Unit.assert-equals (Reader.parse "(const (thread?) *builtin*.thread?)\n")
        (list 
            {   'description () 
                'name "thread?" 
                'properties ()
            }
        )
    )
)

(Unit.test "parse - just a description"
    (Unit.assert-equals (Reader.parse "; This is the description\n(const thread? *builtin*.thread?)\n")
        (list 
            {   'description '("This is the description") 
                'name "thread?" 
                'properties ()
            }
        )
    )
)

(Unit.test "parse - description with a property"
    (const input "; This is the description\n;\n; :usage (thread? socket)\n; :parameter socket socket?\n;   The value to test whether it is a socket\n(const thread? *builtin*.thread?)\n")

    (Unit.assert-equals (Reader.parse input)
        (list 
            {   'description '("This is the description") 
                'name "thread?" 
                'properties '(
                    {   type "usage"
                        signature "(thread? socket)"
                        description ()
                    }
                    {   type "parameter"
                        name "socket"
                        signature "socket?"
                        description ("The value to test whether it is a socket")
                    }
                )
            }
        )
    )
)
