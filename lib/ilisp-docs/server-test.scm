(import "../unit.scm" :as Unit)
(import "./server.scm" :as Server)

(const- test-content "GET /api/doc?/workspace/ilisp/lib/ilisp-docs/reader.scm HTTP/1.1
Host: localhost:8080
Upgrade-Insecure-Requests: 1
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_6) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/14.0.3 Safari/605.1.15
Accept-Language: en-us
Accept-Encoding: gzip, deflate
Connection: keep-alive


")

(Unit.test "header-content"
    (Unit.assert-equals (Server.header-content test-content) "GET /api/doc?/workspace/ilisp/lib/ilisp-docs/reader.scm HTTP/1.1
Host: localhost:8080
Upgrade-Insecure-Requests: 1
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_6) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/14.0.3 Safari/605.1.15
Accept-Language: en-us
Accept-Encoding: gzip, deflate
Connection: keep-alive")
)

(Unit.test "parse-property-value"
    (Unit.assert-equals (Server.parse-value-property "Host: localhost:8080") (list "Host" "localhost:8080"))
)

(Unit.test "parse-request-line"
    (Unit.assert-equals (Server.parse-request-line "GET /api/doc HTTP/1.1") ['GET "/api/doc" () "HTTP/1.1"])
    (Unit.assert-equals (Server.parse-request-line "GET /api/doc?/workspace/ilisp/lib/ilisp-docs/reader.scm HTTP/1.1") ['GET "/api/doc" "/workspace/ilisp/lib/ilisp-docs/reader.scm" "HTTP/1.1"])
)

(Unit.test "header-properties"
    (Unit.assert-equals (Server.header-properties test-content) 
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
)
