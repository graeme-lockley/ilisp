; Construct an atom instance
;
; :usage (atom value)
; :parameter value any?
; :returns atom?
;   An atom containing `value`.
(const atom *builtin*.atom)

; Returns the first element of a pair.
;
; :usage (pair-car value)
; :parameter value pair?
; :returns any?
; :signal InvalidArgument
;   `value` is not a pair.
(const pair-car *builtin*.pair-car)

; Returns the second element of a pair.
;
; :usage (pair-cdr value)
; :parameter value pair?
; :returns any?
; :signal InvalidArgument
;   `value` is not a pair.
(const pair-cdr *builtin*.pair-cdr)

; Determine whether or not the passed value is an integer.
;
; :usage (integer? value)
; :parameter value any?
;   The value to test.
; :returns boolean?
;   `#t` if `value` refers to an integer otherwise `#f`.
(const integer? *builtin*.integer?)

; Determine whether or not the passed value is a pair.
;
; :usage (pair? value)
; :parameter value any?
;   The value to test.
; :returns boolean?
;   `#t` if `value` refers to a pair otherwise `#f`.
(const pair? *builtin*.pair?)

; Determine whether or not the passed value is a thread.
;
; :usage (thread? value)
; :parameter value any?
;   The value to test.
; :returns boolean?
;   `#t` if `value` refers to a thread otherwise `#f`.
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

; Wait for the referenced thread to complete.
;
; :usage (thread-join thread)
; :parameter thread thread?
;   The running thread which the calling thread is to wait for.
; :returns any?
;   The result returned from the running thread.
; :signal SystemError
;   This signal is raised should the call fail.
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
