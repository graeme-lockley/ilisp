; Construct an atom instance
;
; :usage (atom value)
; :parameter value any?
; :returns atom?
;   An atom containing `value`.
(const atom *builtin*.atom)

; Construct a mutable map from the passed arguments.
;
; :usage (mutable-map . values)
; :parameter values list?
; :returns mutable-map?
;   A mutable map composed of elements from `values`.
(const mutable-map *builtin*.mutable-map)

; Construct a mutable vector from the passed arguments.
;
; :usage (mutable-vector . values)
; :parameter values list?
; :returns mutable-vector?
;   A mutable vector composed of elements from `values`.
(const mutable-vector *builtin*.mutable-vector)

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

; Determine whether or not the passed value is null.
;
; :usage (null? value)
; :parameter value any?
;   The value to test.
; :returns boolean?
;   `#t` if `value` refers is null otherwise `#f`.
(const null? *builtin*.null?)

; Determine whether or not the passed value is an integer.
;
; :usage (integer? value)
; :parameter value any?
;   The value to test.
; :returns boolean?
;   `#t` if `value` refers to an integer otherwise `#f`.
(const integer? *builtin*.integer?)

; Determine whether or not the passed value is a boolean.
;
; :usage (boolean? value)
; :parameter value any?
;   The value to test.
; :returns boolean?
;   `#t` if `value` refers to a boolean otherwise `#f`.
(const boolean? *builtin*.boolean?)

; Determine whether or not the passed value is a character.
;
; :usage (character? value)
; :parameter value any?
;   The value to test.
; :returns boolean?
;   `#t` if `value` refers to a character otherwise `#f`.
(const character? *builtin*.character?)

; Determine whether or not the passed value is a byte-vector.
;
; :usage (byte-vector? value)
; :parameter value any?
;   The value to test.
; :returns boolean?
;   `#t` if `value` refers to a byte-vector otherwise `#f`.
(const byte-vector? *builtin*.byte-vector?)

; Determine whether or not the passed value is a pair.
;
; :usage (pair? value)
; :parameter value any?
;   The value to test.
; :returns boolean?
;   `#t` if `value` refers to a pair otherwise `#f`.
(const pair? *builtin*.pair?)

; Determine whether or not the passed value is a string.
;
; :usage (string? value)
; :parameter value any?
;   The value to test.
; :returns boolean?
;   `#t` if `value` refers to a string otherwise `#f`.
(const string? *builtin*.string?)

(const map? *builtin*.map?)

; Mutate `map` by incorporating the passed bindings.
; 
; :usage (map-assoc! map . k1 v1 ... kn vn)
; :parameter map mutable-map?
; :parameter k1 any?
;   The key of the first binding.
; :parameter v1 any?
;   The value of the first binding.
; :parameter kn any?
;   The key of the last binding.
; :parameter vn any?
;   The value of the last binding.
(const map-assoc! *builtin*.map-assoc!)

; Verifies whether or not `key` is contained in `map`.
;
; :usage (map-contains? map key)
; :parameter map map?
; :parameter key any?
; :returns boolean?
;   `#t` if `key` in `map` otherwise `#f`.
(const map-contains? *builtin*.map-contains?)

; Determine whether or not the passed value is a thread.
;
; :usage (thread? value)
; :parameter value any?
;   The value to test.
; :returns boolean?
;   `#t` if `value` refers to a thread otherwise `#f`.
(const thread? *builtin*.thread?)

; Converts a string to a byte-vector.
;
; :usage (string->byte-vector s)
; :parameter s string?
;   The string to be converted
; :returns byte-vector?
;   Note that the lenghts of `s` and the result are the same.
; :signal InvalidArgument
;   The argument `s` is not a string
(const string->byte-vector *builtin*.string->byte-vector)

(const byte-vector-nth *builtin*.byte-vector-nth)

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
