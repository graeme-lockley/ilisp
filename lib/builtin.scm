(const thread? *builtin*.thread?)

; Creates and returns a new thread.  The new thread invokes the passed thunk 
; within the new thread.  The new thread executes within the enclosing closure.
;
; :usage (thread-create p)
; :parameter p procedure?
;   The procedure that is invoked without any arguments.
; :returns thread?
;   A reference to the newly created thread.
;
; :signal SystemError
;   This signal is raised should the attempt to create a new thread fails.
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
