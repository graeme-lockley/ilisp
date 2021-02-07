(import "../lib/filesystem.scm" :as FS)
(import "../lib/list.scm" :as List)
(import "../lib/string.scm" :as S)

(define test-files
    (map
        (List.filter  
          (FS.read-dir (FS.absolute-name-relative-to *source-name* "../lib"))
          (fn (t) (and (get t :file?) (S.ends-with (get t :name) "-test.scm")))
        )
        (fn (t) (get t :name))
    )
)

(for-each test-files (fn (name)
    (do 
        (define qualified-name (str "../lib/" name))

        (println "Running tests: " name)
        (package-import qualified-name)
    )
))
