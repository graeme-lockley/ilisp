(import "../lib/filesystem.scm" :as FS)
(import "../lib/unit.scm" :as Unit)

(const- (directory dir)
    (do
        (const (format-name n) (str dir "/" (get n :name)))
        (const (file-is-dir? n) (get n :dir?))
        (const (file-is-test? n) (and (get n :file?) (ends-with (get n :name) "-test.scm")))

        (const contents (FS.read-dir (FS.absolute-name-relative-to-file-name *source-name* dir)))

        (const directories (map (filter contents file-is-dir?) format-name))
        (const files-in-directories (map directories directory))
 
        (const files (map (filter contents file-is-test?) format-name))

        (const files-with-absolute-name (map files (proc (n) (FS.absolute-name-relative-to-file-name *source-name* n))))
        
        (apply concat (cons files-with-absolute-name files-in-directories))
    )
)


(const- test-files (directory ".."))

(const- *signal* #f)

(const- (show-test-stats)
    (prn (Unit.stats))
)

(for-each test-files (proc (name)
    (do 
        (println "Running tests: " name)
        (try
            (package-import name)
            (proc (s)
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
