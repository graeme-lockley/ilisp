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
    (const idx (Sequence.index-of content "\n\n"))

    (if (*builtin*.null? idx) (raise 'InvalidHTTPHeaderContent {:reason "No blank line in content" :content content})
        (slice content 0 (- (car idx) 1))
    )
)

(const (parse-value-property line)
    (const values (String.split-with line (proc (c) (not (= c #\:)))))

    (list (car values) (String.drop (cdr values) 2))
)

(const (index-of haystack needle)
    (const s-haystack (count haystack))

    (const (find-needle idx)
        (if (= idx s-haystack) ()
            (= (nth haystack idx) needle) idx
            (find-needle (+ idx 1))
        )
    )

    (find-needle 0)
)

(const (parse-request-line line)
    (const words (Sequence.split line " "))

    (const index-of-question (index-of (nth words 1) #\?))

    (if (*builtin*.null? index-of-question) [(*builtin*.symbol (car words)) (nth words 1) () (nth words 2)]
        [(*builtin*.symbol (car words)) (take (nth words 1) index-of-question) (drop (nth words 1) (+ index-of-question 1)) (nth words 2)]
    )
)

(const (header-properties content)
    (const hc (header-content content))
    (const lines (Sequence.split hc "\x0a;"))

    (const (parse-value-properties lines)
        (const values (*builtin*.apply concat (List.map lines parse-value-property)))

        (*builtin*.apply *builtin*.mk-map values)
    )

    (pair (parse-request-line (car lines)) (parse-value-properties (cdr lines)))
)

(const (main)
    (const ss (*builtin*.socket-listen listener 8080))
    (*builtin*.thread-join (car ss))
)
