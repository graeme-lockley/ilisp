(import "./builtin.scm" :as Builtin)

; Construct an atom instance
;
; :usage (atom value)
; :parameter value any?
; :returns atom?
;   An atom containing `value`.
(const atom Builtin.atom)
