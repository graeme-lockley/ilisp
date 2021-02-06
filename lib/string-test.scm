(import "./string.scm" :as S)

(import "./unit.scm" :as Unit)

(define t (=))

(Unit.assert-equals (S.ends-with "" "") t)
(Unit.assert-equals (S.ends-with "hello" "") t)
(Unit.assert-equals (S.ends-with "hello" "lo") t)
(Unit.assert-equals (S.ends-with "hello" "hello") t)

(Unit.assert-equals (S.ends-with "hello" "Lo") ())
(Unit.assert-equals (S.ends-with "hello" "ohello") ())
