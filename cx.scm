(const (listener p) 
    (const d (*builtin*.socket-read-all p)) 
    (println "Server: " (*builtin*.byte-vector->string d))
    (*builtin*.socket-write-all p (*builtin*.string->byte-vector "Hello world"))
    (*builtin*.socket-close p) 
)
(const ss (*builtin*.socket-listen listener 8080))

; (const s (*builtin*.socket-open "127.0.0.1" 8080))
; (*builtin*.socket-write-all s (*builtin*.string->byte-vector "Bye bye love"))
; (println "Client: " (*builtin*.byte-vector->string (*builtin*.socket-read-all s)))
; (*builtin*.socket-close s)

(*builtin*.thread-join (car ss))
