(import "./parser.scm" :as Parser)
(import "./scanner.scm" :as Scanner)
(import "./static.scm" :names ast->tst)
(import "./tst.scm" :as TST)

(import "../unit.scm" :as Unit)

(const string->tst
    (|> Scanner.string->scanner Parser.scanner->ast ast->tst)
)

(Unit.test "error - unknown identifier"
    (Unit.assert-signal-name (string->tst "y") 'UnknownIdentifier)
)
