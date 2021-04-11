(const thread? *builtin*.thread?)

(const thread-create *builtin*.thread-create)

(const thread-join *builtin*.thread-join)

(const socket-listen *builtin*.socket-listen)

; :returns (and? byte-vector? immutable?)
;
; :signal `SystemError`
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


;; [
;;     {
;;         name: "socket-write-all",
;;         description: "Synchronously write `byte` to `socket`."
;;         properties:
;;             [
;;                 {
;;                     type: "usage",
;;                     description: "(socket-write-all socket bytes)"
;;                 },
;;                 {
;;                     type: "parameter", 
;;                     name: "socket",
;;                     signature: "socket?",
;;                     description: "The socket to write `bytes` to"
;;                 },
;;                 {
;;                     type: "parameter", 
;;                     name: "bytes",
;;                     signature: "byte-vector?",
;;                     description: "The bytes to be written to `socket`."
;;                 },
;;                 {
;;                     type: "returns", 
;;                     name: "SystemError",
;;                     description: "A system error has occurred."
;;                 }
;;             ]
;;     }
;; ]
