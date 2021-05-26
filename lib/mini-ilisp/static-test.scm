(import "./parser.scm" :as Parser)
(import "../predicate.scm" :names null?)
(import "./scanner.scm" :as Scanner)
(import "./static.scm" :names ast->tst)

(import "../unit.scm" :as Unit)

(const (|> . fs)
    (const (apply-rest fs' v)
        (if (null? fs') v
            (apply-rest (cdr fs') ((car fs') v))
        )
    )

    (proc (v)
        (apply-rest fs v)
    )
)

(const string->tst
    (|> Scanner.string->scanner Parser.scanner->ast ast->tst)
)

(Unit.test "error - unknown identifier"
    (Unit.assert-signal-name
        (string->tst "y")
        'UnknownIdentifier
    )
)
