(import "../lib/filesystem.scm" :as FS)
(import "../lib/unit.scm" :as Unit)

(define (directory dir)
    (do
        (define (format-name n) (str dir "/" (get n :name)))
        (define (file-is-dir? n) (get n :dir?))
        (define (file-is-test? n) (and (get n :file?) (ends-with (get n :name) "-test.scm")))

        (define contents (FS.read-dir (FS.absolute-name-relative-to-file-name *source-name* dir)))

        (define directories (map (filter contents file-is-dir?) format-name))
        (define files-in-directories (map directories directory))
 
        (define files (map (filter contents file-is-test?) format-name))

        (define files-with-absolute-name (map files (fn (n) (FS.absolute-name-relative-to-file-name *source-name* n))))
        
        (apply concat (cons files-with-absolute-name files-in-directories))
    )
)

(define test-files (directory "../lib"))

(define *signal* ())

(define (show-test-stats)
    (prn (Unit.stats))
)

(for-each test-files (fn (name)
    (do 
        (println "Running tests: " name)
        (try
            (package-import name)
            (fn (s)
                (do
                    (print "Test failed: ")
                    (prn s)
                    (set! *signal* s)
                )
            )
        )
    )
))

(show-test-stats)

(if *signal* (raise *signal*))
