(import "./server.scm" :as Server)
(import "../string.scm" :as String)
(import "../unit.scm" :as Unit)

(Unit.test "first-index-of"
    (Unit.assert-equals (Server.first-index-of "hello world" "") (pair 0 ""))
    (Unit.assert-equals (Server.first-index-of "hello world" ()) ())

    (Unit.assert-equals (Server.first-index-of "hello world" #\z) ())
    (Unit.assert-equals (Server.first-index-of "hello world" '(#\z)) ())
    (Unit.assert-equals (Server.first-index-of "hello world" #\l) (pair 2 #\l))
    (Unit.assert-equals (Server.first-index-of "hello world" '(#\l)) (pair 2 #\l))
    (Unit.assert-equals (Server.first-index-of "hello world" "lo") (pair 3 "lo"))
    (Unit.assert-equals (Server.first-index-of "hello world" '("lo")) (pair 3 "lo"))
    (Unit.assert-equals (Server.first-index-of "hello world" "ld") (pair 9 "ld"))
    (Unit.assert-equals (Server.first-index-of "hello world" "lz") ())
    (Unit.assert-equals (Server.first-index-of "hello world" '("lz")) ())

    (Unit.assert-equals (Server.first-index-of "hello world" '("lo" "ld" #\l)) (pair 2 #\l))
    (Unit.assert-equals (Server.first-index-of "hello world" '("lo" "ld")) (pair 3 "lo"))
)

(Unit.test "first-index-only-of"
    (Unit.assert-equals (Server.first-index-only-of "hello world" "") 0)
    (Unit.assert-equals (Server.first-index-only-of "hello world" ()) ())
    (Unit.assert-equals (Server.first-index-only-of "hello world" #\z) ())
    (Unit.assert-equals (Server.first-index-only-of "hello world" '(#\z)) ())
    (Unit.assert-equals (Server.first-index-only-of "hello world" #\l) 2)
    (Unit.assert-equals (Server.first-index-only-of "hello world" '(#\l)) 2)
    (Unit.assert-equals (Server.first-index-only-of "hello world" "lo") 3)
    (Unit.assert-equals (Server.first-index-only-of "hello world" '("lo")) 3)
    (Unit.assert-equals (Server.first-index-only-of "hello world" "ld") 9)
    (Unit.assert-equals (Server.first-index-only-of "hello world" "lz") ())
    (Unit.assert-equals (Server.first-index-only-of "hello world" '("lz")) ())
    (Unit.assert-equals (Server.first-index-only-of "hello world" '("lo" "ld" #\l)) 2)
    (Unit.assert-equals (Server.first-index-only-of "hello world" '("lo" "ld")) 3)
)

(const- test-content 
    '(  "GET /api/doc?/workspace/ilisp/lib/ilisp-docs/reader.scm HTTP/1.1"
        "Host: localhost:8080"
        "Upgrade-Insecure-Requests: 1"
        "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8"
        "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_6) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/14.0.3 Safari/605.1.15"
        "Accept-Language: en-us"
        "Accept-Encoding: gzip, deflate"
        "Connection: keep-alive"
        ""
        ""
    )
)

(Unit.test "header-content"
    (const result 
        '(  "GET /api/doc?/workspace/ilisp/lib/ilisp-docs/reader.scm HTTP/1.1"
            "Host: localhost:8080"
            "Upgrade-Insecure-Requests: 1"
            "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8"
            "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_6) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/14.0.3 Safari/605.1.15"
            "Accept-Language: en-us"
            "Accept-Encoding: gzip, deflate"
            "Connection: keep-alive"
        )
    )

    (Unit.assert-equals (Server.header-content (String.interpolate-with test-content #xa)) (String.interpolate-with result #xa))
    (Unit.assert-equals (Server.header-content (String.interpolate-with test-content "\xd;\xa;")) (String.interpolate-with result "\xd;\xa;"))
)

(Unit.test "parse-property-value"
    (Unit.assert-equals (Server.parse-value-property "Host: localhost:8080") (list "Host" "localhost:8080"))
)

(Unit.test "parse-request-line"
    (Unit.assert-equals (Server.parse-request-line "GET /api/doc HTTP/1.1") ['GET "/api/doc" () "HTTP/1.1"])
    (Unit.assert-equals (Server.parse-request-line "GET /api/doc?/workspace/ilisp/lib/ilisp-docs/reader.scm HTTP/1.1") ['GET "/api/doc" "/workspace/ilisp/lib/ilisp-docs/reader.scm" "HTTP/1.1"])
)

(Unit.test "header-properties"
    (Unit.assert-equals (Server.header-properties (String.interpolate-with test-content "\xd;\xa;")) 
        (pair 
            ['GET "/api/doc" "/workspace/ilisp/lib/ilisp-docs/reader.scm" "HTTP/1.1"]
            {
                "Host" "localhost:8080"
                "Upgrade-Insecure-Requests" "1"
                "Accept" "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8"
                "User-Agent" "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_6) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/14.0.3 Safari/605.1.15"
                "Accept-Language" "en-us"
                "Accept-Encoding" "gzip, deflate"
                "Connection" "keep-alive"
            }
        )
    )

    (const c' "GET / HTTP/1.1
Host: localhost:8080
Upgrade-Insecure-Requests: 1
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_6) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/14.0.3 Safari/605.1.15
Accept-Language: en-us
Accept-Encoding: gzip, deflate
Connection: keep-alive

")

    (Unit.assert-equals (Server.header-properties c') 
        (pair 
            ['GET "/" () "HTTP/1.1"]
            {
                "Host" "localhost:8080"
                "Upgrade-Insecure-Requests" "1"
                "Accept" "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8"
                "User-Agent" "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_6) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/14.0.3 Safari/605.1.15"
                "Accept-Language" "en-us"
                "Accept-Encoding" "gzip, deflate"
                "Connection" "keep-alive"
            }
        )
    )
)
