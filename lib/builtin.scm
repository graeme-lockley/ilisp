(const thread? *builtin*.thread?)

(const thread-create *builtin*.thread-create)

(const thread-join *builtin*.thread-join)

(const socket-listen *builtin*.socket-listen)

; :returns (and? byte-vector? immutable?)
;
; :signal SystemError
;   A system error has occurred.
(const socket-read-all *builtin*.socket-read-all)

; Synchronously write `byte` to `socket`.
;
; :usage (socket-write-all socket bytes)
; :parameter socket socket?
;   The socket to write `bytes` to
; :parameter bytes byte-vector?
;   The bytes to be written to `socket`.
; :returns null?
;
; :signal SystemError
;   A system error has occurred.
(const socket-write-all *builtin*.socket-write-all)
