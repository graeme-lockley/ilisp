(import "../filesystem.scm" :as FS)
(import "./json.scm" :as JSON)
(import "../list.scm" :as List)
(import "./reader.scm" :as Reader)
(import "../string.scm" :as String)
(import "../sequence.scm" :as Sequence)

; Extracts the header text from `content`.  Should the content not contain a
; blank line then the signal `InvalidHTTPHeaderContent` is raised.
;
; :usage (header-content content)
; :parameter content string?
;   The entire content received via an HTTP request.
; :returns string?
;   The header content excluding the blank line.
; 
; :signal InvalidHTTPHeaderContent
;   The content contained no blank line.  The first blank signals the end of
;   the header and the start of the body.
(const (header-content content) 
    (const idx (first-index-of content '("\x0d;\x0a;\x0d;\x0a;" "\x0a;\x0a;")))

    (if (*builtin*.null? idx) (raise 'InvalidHTTPHeaderContent {:reason "No blank line in content" :content content})
        (slice content 0 (- (car idx) 1))
    )
)

(const (parse-value-property line)
    (const values (String.split-with line (proc (c) (not (= c #\:)))))

    (list (car values) (String.drop (cdr values) 2))
)

; Returns an index and needle into `haystack` of the first occurence of an 
; element within `needles`.
;
; :usage (first-index-of haystack needles)
; :parameter haystack string?
;   A string into which we are search for the element from needles
; :parameter needles (or? string? character? (list-of? (or? string? character?)))
;   Needles can consist either of a single string or character or a list of 
;   strings and characters.
; :return (or? (pair-of? number? (or? string? character?)) null?)
;   If a needle cannot be found in `haystack` returns `()` otherwise returns the
;   index into `needle` and the specific needle that was used in the match.
;
; :assert-equals (first-index-of "hello world" "") (0 . "")
; :assert-equals (first-index-of "hello world" ()) ()
; :assert-equals (first-index-of "hello world" #\z) ()
; :assert-equals (first-index-of "hello world" '(#\z)) ()
; :assert-equals (first-index-of "hello world" #\l) (2 . #\l)
; :assert-equals (first-index-of "hello world" '(#\l)) (2 . #\l)
; :assert-equals (first-index-of "hello world" "lo") (3 . "lo")
; :assert-equals (first-index-of "hello world" '("lo")) (3 . "lo")
; :assert-equals (first-index-of "hello world" "ld") (9 . "ld")
; :assert-equals (first-index-of "hello world" "lz") ()
; :assert-equals (first-index-of "hello world" '("lz")) ()
; :assert-equals (first-index-of "hello world" '("lo" "ld" #\l)) (2 . #\l)
; :assert-equals (first-index-of "hello world" '("lo" "ld")) (3 . "lo")
(const (first-index-of haystack needles)
    (const s-haystack (count haystack))

    (const (string-= idx needle)
        (const s-needle (count needle))

        (const (loop idx needle-idx)
            (if (= needle-idx s-needle) #t
                (= idx s-haystack) #f
                (= (nth haystack idx) (nth needle needle-idx)) (loop (+ idx 1) (+ needle-idx 1))
                #f
            )
        )

        (loop idx 0)
    )

    (const (index-of-a-character)
        (const (find-needle idx)
            (if (= idx s-haystack) ()
                (= (nth haystack idx) needles) (pair idx needles)
                (find-needle (+ idx 1))
            )
        )

        (find-needle 0)
    )

    (const (index-of-a-string)
        (const (find-needle idx)
            (if (= idx s-haystack) ()
                (string-= idx needles) (pair idx needles)
                (find-needle (+ idx 1))
            )
        )

        (find-needle 0)
    )

    (const (index-of-a-list)
        (const s-needles (count needles))

        (const (loop haystack-idx needles-idx)
            (const (needle-on-index)
                (const needle (nth needles needles-idx))

                (if (*builtin*.character? needle) (if (= (nth haystack haystack-idx) needle) (pair haystack-idx needle) ())
                    (*builtin*.string? needle) (if (string-= haystack-idx needle) (pair haystack-idx needle) ())
                    (raise 'InvalidArgument {:procedure 'first-index-of :arg-number 0 :expected-type (list 'pair () 'string 'character) :received needles})
                )
            )

            (if (= haystack-idx s-haystack) ()
                (= needles-idx s-needles) (loop (+ haystack-idx 1) 0)
                (do 
                    (const result (needle-on-index))
                    (if (*builtin*.null? result) (loop haystack-idx (+ needles-idx 1))
                        result
                    )
                )
            )
        )

        (loop 0 0)
    )

    (if (*builtin*.character? needles) (index-of-a-character)
        (*builtin*.string? needles) (index-of-a-string)
        (List.list? needles) (index-of-a-list)
        (raise 'InvalidArgument {:procedure 'first-index-of :arg-number 0 :expected-type (list 'pair () 'string 'character) :received needles})
    )
)

; Returns an index into `haystack` of the first occurence of an element within
; `needles`.
;
; :usage (first-index-only-of haystack needles)
; :parameter haystack string?
;   A string into which we are search for the element from needles
; :parameter needles (or? string? character? (list-of? (or? string? character?)))
;   Needles can consist either of a single string or character or a list of 
;   strings and characters.
; :return (or? number? null?)
;   If a needle cannot be found in `haystack` returns `()` otherwise returns the
;   index into `needle`.
;
; :assert-equals (first-index-only-of "hello world" "") 0
; :assert-equals (first-index-only-of "hello world" ()) ()
; :assert-equals (first-index-only-of "hello world" #\z) ()
; :assert-equals (first-index-only-of "hello world" '(#\z)) ()
; :assert-equals (first-index-only-of "hello world" #\l) 2
; :assert-equals (first-index-only-of "hello world" '(#\l)) 2
; :assert-equals (first-index-only-of "hello world" "lo") 3
; :assert-equals (first-index-only-of "hello world" '("lo")) 3
; :assert-equals (first-index-only-of "hello world" "ld") 9
; :assert-equals (first-index-only-of "hello world" "lz") ()
; :assert-equals (first-index-only-of "hello world" '("lz")) ()
; :assert-equals (first-index-only-of "hello world" '("lo" "ld" #\l)) 2
; :assert-equals (first-index-only-of "hello world" '("lo" "ld")) 3
(const (first-index-only-of haystack needle)
    (const result (first-index-of haystack needle))
    (if (*builtin*.null? result) ()
        (car result)
    )
)

(const (parse-request-line line)
    (const words (Sequence.split line " "))

    (const index-of-question (first-index-only-of (nth words 1) #\?))

    (if (*builtin*.null? index-of-question) [(*builtin*.symbol (car words)) (nth words 1) () (nth words 2)]
        [(*builtin*.symbol (car words)) (take (nth words 1) index-of-question) (drop (nth words 1) (+ index-of-question 1)) (nth words 2)]
    )
)

(const (header-properties content)
    (const hc (header-content content))
    (const lines (Sequence.split hc '("\x0d;\x0a;" "\x0a;")))

    (const (parse-value-properties lines)
        (const values (*builtin*.apply concat (List.map lines parse-value-property)))

        (*builtin*.apply *builtin*.mk-map values)
    )

    (pair (parse-request-line (car lines)) (parse-value-properties (cdr lines)))
)

(const (header-path header)
    (request-line-path (car header))
)

(const (http-server listener port)
    (*builtin*.socket-listen listener port)
)

(const (request-line-verb request-line) (nth request-line 0))

(const (request-line-path request-line) (nth request-line 1))

(const (request-line-query-string request-line) (nth request-line 2))

(const (route-verb route) (nth route 0))

(const (route-path route) (nth route 1))

; Locates the first route within `routes` based on the path captured in 
; `request-line` and returns that route with the parameters extracted out of
; path and query string.
;
; If no route matches against the `request-line` the this procedure returned `#f`.
;
; :usage (find-round routes request-line)
; :parameter routes (list-of? route?)
; :parameter request-line request-line?
; :returns (or? (=? #f) (pair-of? route? (map-of? symbol? string?)))
(const (find-route routes request-line)
    (const (find-first s p)
        (if (*builtin*.null? s) #f
            (do (const possible-result (p (car s)))
                (if possible-result possible-result (find-first (cdr s) p))
            )
        )
    )

    (const (route-equals route)
        (const path (request-line-path request-line))
        (const index-of-lcurley (first-index-only-of (route-path route) "{"))

        (if (*builtin*.null? index-of-lcurley)
                (if (and (= (route-verb route) (request-line-verb request-line))
                         (= (route-path route) path)) (pair route {})
                    #f
                )
            (and (= (route-verb route) (request-line-verb request-line))
                 (= (take path index-of-lcurley) (take (route-path route) index-of-lcurley))) 
                (do (const name (drop-right (drop (route-path route) (+ index-of-lcurley 1)) 1))
                    (const value (drop path index-of-lcurley))

                    (pair route {(*builtin*.symbol name) value})
                )
            #f
        )
    )

    (find-first routes route-equals)
)

(const (http-result code content-type content)
    ['HTTP-RESULT code content-type content]
)

(const (http-result? v)
    (and (*builtin*.vector? v)
         (= (nth v 0) 'HTTP-RESULT)
    )
)

(const (http-result-code v) (nth v 1))

(const (http-result-content-type v) (nth v 2))

(const (http-result-content v) (nth v 3))

(const (socket-write-http-result s v)
    (if (not (*builtin*.socket? s))
        (raise 'InvalidArgument {:procedure 'socket-write-http-result :arg-number 0 :expected-type (list 'socket) :received s})
    )

    (if (not (http-result? v))
        (raise 'InvalidArgument {:procedure 'socket-write-http-result :arg-number 1 :expected-type (list 'http-result) :received v})
    )

    (const content (http-result-content v))
    (const raw-content
        (if (*builtin*.string? content) (*builtin*.string->byte-vector content)
            content
        )
    )

    (*builtin*.socket-write-all s (*builtin*.string->byte-vector (str
        "HTTP/1.1 " (http-result-code v) " OK\x0d;\x0a;"
        "Content-Type: " (http-result-content-type v) "\x0d;\x0a;"
        "Content-Length: " (count raw-content) "\x0d;\x0a;"
        "\x0d;\x0a;"
    )))

    (*builtin*.socket-write-all s raw-content)
    (*builtin*.socket-write-all s (*builtin*.string->byte-vector "\x0d;\x0a;"))
)

(const- (not-found-error p route)
    (socket-write-http-result p (http-result 404 "text/plain" (pr-str route)))
)

(const (http-listener routes)
    (const (listener p)
        (try
            (do 
                (const content (*builtin*.byte-vector->string (*builtin*.socket-read-all p)))

                (const header (header-properties content))

                (const request-line (car header))

                (const route-result (find-route routes request-line))

                (if route-result 
                        (do (const route (car route-result))
                            (const parameters (cdr route-result))
                            
                            (socket-write-http-result p ((nth route 2) parameters (request-line-query-string request-line) header))
                        )
                    (not-found-error p request-line)
                )
            )
            (proc (signal)
                (try
                    (socket-write-http-result p (http-result 500 "application/json" (JSON.->string signal)))

                    (proc (signal')
                        (socket-write-http-result p (http-result 500 "text/plain" signal))
                    )
                )
            )
        )

        (*builtin*.socket-close p) 
    )

    listener
)

(const mime-types
    {   ".html" "text/html"
        ".css" "text/css"
    }
)

(const default-mime-type "text/text")

(const (mime-type name)
    (const index-of-period (first-index-only-of name "."))

    (if (*builtin*.null? index-of-period) default-mime-type
        (do (const extension (drop name index-of-period))
            (const mime-type (map-find mime-types extension))

            (if (*builtin*.null? mime-type) default-mime-type
                mime-type
            )
        )
    )
)

(const (main)
    (const (get-api-doc parameters query-string header-fields)
        (println "get-api-doc: " query-string)

        (const read-value (Reader.parse (*builtin*.slurp (*builtin*.file-name-relative-to-file-name *source-name* query-string))))
        (const result (JSON.->string read-value))

        (http-result 200 "application/json" result)
    )

    (const (get-api-index base-dir)
        (proc (parameters query-string header)
            (const (directory dir)
                (const (format-name n) (str dir "/" (map-get n :name)))
                (const (file-is-dir? n) (map-get n :dir?))
                (const (file-is-ilisp? n) (and (map-get n :file?) (ends-with (map-get n :name) ".scm")))

                (const contents (FS.read-dir (FS.absolute-name-relative-to-file-name *source-name* dir)))

                (const directories (*builtin*.list-map (filter contents file-is-dir?) format-name))
                (const files-in-directories (*builtin*.list-map directories directory))

                (const files (*builtin*.list-map (filter contents file-is-ilisp?) format-name))

                (const files-with-absolute-name (*builtin*.list-map files (proc (n) (FS.absolute-name-relative-to-file-name *source-name* n))))
                
                (*builtin*.apply concat (*builtin*.pair files-with-absolute-name files-in-directories))
            )

            (const base-dir-count (count base-dir))

            (println "get-api-index: " base-dir)

            (http-result 200 "application/json" (JSON.->string (*builtin*.list-map (directory base-dir) (proc (n) [n (drop n (+ base-dir-count 1))]))))
        )        
    )

    (const (get-static-file parameters query-string header)
        (const source-name parameters.source-name)

        (println "get-static-file: " source-name)

        (http-result 200 (mime-type source-name) (*builtin*.slurp (str "./lib/ilisp-docs/" source-name)))
    )

    (const listener 
        (http-listener (list 
            ['GET "/api/doc" get-api-doc]
            ['GET "/api/index" (get-api-index (str *env*.PWD "/lib"))]
            ['GET "/public/{source-name}" get-static-file]
        ))
    )

    (const ss (http-server listener 8080))
    (*builtin*.thread-join (car ss))
)
