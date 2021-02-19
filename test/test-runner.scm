(import "../lib/filesystem.scm" :as FS)
(import "../lib/unit.scm" :as Unit)

(define test-files
    (map
        (filter  
          (FS.read-dir (FS.absolute-name-relative-to *source-name* "../lib"))
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
        (define qualified-name (str "../lib/" name))

        (println "Running tests: " name)
        (try
            (package-import qualified-name)
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
