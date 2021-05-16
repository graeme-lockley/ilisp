(import "../unit.scm" :as Unit)
(import "../predicate.scm" :names integer? string?)
(import "../data/struct.scm" :names mutable-struct struct)

(Unit.test "single field"
    (struct Single
        (value integer?)
    )

    (const c (Single 1))

    (Unit.assert-equals (Single-value c) 1)
    (Unit.assert-truthy (Single? c))
    (Unit.assert-falsy (Single? 1))

    (Unit.assert-signal-name (Single "hello") 'IllegalArgument)
)

(Unit.test "multiple field"
    (struct Multiple
        (int integer?)
        (str string?)
    )

    (const c (Multiple 1 "hello"))

    (Unit.assert-equals (Multiple-int c) 1)
    (Unit.assert-equals (Multiple-str c) "hello")
    (Unit.assert-truthy (Multiple? c))
    (Unit.assert-falsy (Multiple? 1))

    (Unit.assert-signal-name (Multiple "hello" 1) 'IllegalArgument)
)

(Unit.test "mutable multiple fields"
    (mutable-struct MMultiple
        (int integer?)
        (str string?)
    )

    (const c (MMultiple 1 "hello"))

    (Unit.assert-equals (MMultiple-int c) 1)
    (Unit.assert-equals (MMultiple-str c) "hello")
    (Unit.assert-truthy (MMultiple? c))
    (Unit.assert-falsy (MMultiple? 1))

    (MMultiple-int! c 2)
    (MMultiple-str! c "world")

    (Unit.assert-equals (MMultiple-int c) 2)
    (Unit.assert-equals (MMultiple-str c) "world")

    (Unit.assert-signal-name (MMultiple-int! c "oops") 'IllegalArgument)
    (Unit.assert-signal-name (MMultiple-str! c #t) 'IllegalArgument)
)