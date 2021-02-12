(export count (get (car **root**) :builtins 'string-count))

(export (drop v n)
    (slice v n (count v))
)

(export ends-with (get (car **root**) :builtins 'string-ends-with))

(export filter (get (car **root**) :builtins 'string-filter))

(export nth (get (car **root**) :builtins 'string-nth))

(export slice (get (car **root**) :builtins 'string-slice))

(export starts-with (get (car **root**) :builtins 'string-starts-with))

(export (take s n)
    (slice s 0 (- n 1))
)

(export (take-right s n)
    (slice s (- (count s) n) (count s))
)