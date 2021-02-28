(import "../lib/filesystem.scm" :as FS)
(import "../lib/unit.scm" :as Unit)

(define (directory dir)
    (do
        (define contents (FS.read-dir (FS.absolute-name-relative-to-file-name *source-name* dir)))

        (define directories (map (filter contents (fn (t) (get t :dir?))) (fn (n) (str dir "/" (get n :name)))))
 
        (define files 
            (map 
                (filter 
                    contents 
                    (fn (t) (and (get t :file?) (ends-with (get t :name) "-test.scm")))
                )
                (fn (t) (str dir "/" (get t :name)))
            )
        )

        (define ff (map files (fn (n) (FS.absolute-name-relative-to-file-name *source-name* n))))
        
        (define result (apply concat (cons ff (map directories directory))))

        (apply prn result)

        result
    )
)

(define test-files (directory "../lib"))

(define test-files'
    (map
        (filter  
          (FS.read-dir (FS.absolute-name-relative-to-file-name *source-name* "../lib"))
          (fn (t) (and (get t :file?) (ends-with (get t :name) "-test.scm")))
        )
        (fn (t) (get t :name))
    )
)

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
