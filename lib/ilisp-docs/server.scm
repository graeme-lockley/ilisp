(import "./json.scm" :as JSON)
(import "../list.scm" :as List)
(import "./reader.scm" :as Reader)
(import "../string.scm" :as String)
;; (import "../sequence.scm" :as Sequence)

;; --- CONTENT -----------------------
;; GET /api/doc?/workspace/ilisp/lib/ilisp-docs/reader.scm HTTP/1.1
;; Host: localhost:8080
;; Upgrade-Insecure-Requests: 1
;; Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
;; User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_6) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/14.0.3 Safari/605.1.15
;; Accept-Language: en-us
;; Accept-Encoding: gzip, deflate
;; Connection: keep-alive
;;
;; Hello world
;; -----------------------------------

(const (listener p) 
    (const content (*builtin*.byte-vector->string (*builtin*.socket-read-all p)))

    (println "--- CONTENT -----------------------")
    (println content)
    (println "-----------------------------------")

    (const result (JSON.->string (Reader.parse (*builtin*.slurp (*builtin*.file-name-relative-to-file-name *source-name* "../builtin.scm")))))

    (*builtin*.socket-write-all p (*builtin*.string->byte-vector (str
        "HTTP/1.1 200 OK\n"
        "Content-Type: application/json\n"
        "Content-Length: " (count result) "\n"
        "\n"
        result
        "\n"
    )))

    (*builtin*.socket-close p) 
)

; Extracts the header text from `content`.  Should the content not contain a
; blank line then the signal `InvalidHTTPHeaderContent` is raised.
;
; :usage (header-content content)
; :parameter content string?
;   The entire content received via an HTTP request.
; :returns string?
;   The header content excluding the blank line.
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

; Returns an index into `haystack` of the first occurence of an element within
; `needles`.
;
; :usage (first-index-of haystack needles)
; :parameter haystack string?
;   A string into which we are search for the element from needles
; :parameter needles (or? string? character? (list-of? (or? string? character?)))
;   Needles can consist either of a single string or character or a list of 
;   strings and characters.
; :return (or? number? null?)
;   If a needle cannot be found in `haystack` returns `()` otherwise returns the
;   index into `needle`.
;
; :assert-equals (first-index-of "hello world" "") 0
; :assert-equals (first-index-of "hello world" ()) ()
; :assert-equals (first-index-of "hello world" #\z) ()
; :assert-equals (first-index-of "hello world" '(#\z)) ()
; :assert-equals (first-index-of "hello world" #\l) 2
; :assert-equals (first-index-of "hello world" '(#\l)) 2
; :assert-equals (first-index-of "hello world" "lo") 3
; :assert-equals (first-index-of "hello world" '("lo")) 3
; :assert-equals (first-index-of "hello world" "ld") 9
; :assert-equals (first-index-of "hello world" "lz") ()
; :assert-equals (first-index-of "hello world" '("lz")) ()
; :assert-equals (first-index-of "hello world" '("lo" "ld" #\l)) 2
; :assert-equals (first-index-of "hello world" '("lo" "ld")) 3
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

(const (find s p)
    (if (*builtin*.null? s) ()
        (p (car s)) (car s)
        (find (cdr s) p)
    )
)

(const (http-server listener port)
    (*builtin*.socket-listen listener port)
)

(const- (not-found-error p route)
    (const content (pr-str route))

    (*builtin*.socket-write-all p (*builtin*.string->byte-vector (str
        "HTTP/1.1 404 OK\x0d;\x0a;"
        "Content-Type: text/plain\x0d;\x0a;"
        "Content-Length: " (count content) "\x0d;\x0a;"
        "\x0d;\x0a;"
        content
        "\x0d;\x0a;"
    )))
)

(const (http-listener routes)
    (const (listener p)
        (try
            (do 
                (const bv (*builtin*.socket-read-all p))
                (const content (*builtin*.byte-vector->string bv))

                (const header (header-properties content))

                (const request-line (car header))

                (const (route-equals route)
                    (and (= (nth route 0) (nth request-line 0))
                         (= (nth route 1) (nth request-line 1))
                    )
                )

                (const route (find routes route-equals))

                (if (*builtin*.null? route) (not-found-error p request-line)
                    (do (const result-content ((nth route 2) (nth request-line 2) header))
                        (*builtin*.socket-write-all p (*builtin*.string->byte-vector (str
                            "HTTP/1.1 200 OK\x0d;\x0a;"
                            "Content-Type: application/json\x0d;\x0a;"
                            "Content-Length: " (count result-content) "\x0d;\x0a;"
                            "\x0d;\x0a;"
                            result-content
                            "\x0d;\x0a;"
                        )))
                    )
                )
            )
            (proc (signal)
                (try
                    (do (const result (JSON.->string signal))
                        (*builtin*.socket-write-all p (*builtin*.string->byte-vector (str
                            "HTTP/1.1 500 OK\n"
                            "Content-Type: application/json\n"
                            "Content-Length: " (count result) "\n"
                            "\n"
                            result
                            "\n"
                        )))
                    )

                    (proc (signal')
                        (const result (pr-str signal'))

                        (*builtin*.socket-write-all p (*builtin*.string->byte-vector (str
                            "HTTP/1.1 500 OK\n"
                            "Content-Type: text/plain\n"
                            "Content-Length: " (count result) "\n"
                            "\n"
                            result
                            "\n"
                        )))
                    )
                )
            )
        )

        (*builtin*.socket-close p) 
    )

    listener
)

(const (main)
    (const (get-api-doc query-string header-fields)
        (JSON.->string (Reader.parse (*builtin*.slurp (*builtin*.file-name-relative-to-file-name *source-name* query-string))))
    )

    (const listener (http-listener (list ['GET "/api/doc" get-api-doc])))

    (const ss (http-server listener 8080))
    (*builtin*.thread-join (car ss))
)
