(import "../lib/filesystem.scm" :as FS)
(import "../lib/list.scm" :as List)
(import "../lib/string.scm" :as S)

(define test-files
    (map (fn (t) (get t :name))
        (List.filter (fn (t) (and (get t :file?) (S.ends-with (get t :name) "-test.scm"))) (FS.read-dir (FS.absolute-name-relative-to *source-name* "../lib")))
    )
)

(define (process-file name)
    (do 
        (define qualified-name (str "../lib/" name))

        (println "Running tests: " name)
        (package-import qualified-name)
    )
)

(map process-file test-files)
