(import "../unit.scm" :as Unit)
(import "../predicate.scm" :names integer? string?)
(import "../data/struct.scm" :names mutable-struct struct)
(import "../data/union.scm" :names union)

(Unit.test "multiple struct"
    (struct S1
        (int integer?)
        (str string?)
    )

    (struct S2
        (int integer?)
        (str string?)
    )

    (union U
        S1? S2? string?
    )

    (const c1 (U (S1 1 "h")))
    (const c2 (U (S2 2 "e")))
    (const c3 (U "f"))

    (Unit.assert-truthy (U? c1))
    (Unit.assert-truthy (U? c2))
    (Unit.assert-truthy (U? c3))

    (Unit.assert-truthy (U? "g"))
)
