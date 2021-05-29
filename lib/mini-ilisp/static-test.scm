(import "./parser.scm" :as Parser)
(import "./scanner.scm" :as Scanner)
(import "./static.scm" :names CallPrintLn-args CallPrintLn? StringLiteral ast->tst)

(import "../unit.scm" :as Unit)

(const string->tst
    (|> Scanner.string->scanner Parser.scanner->ast ast->tst)
)

(Unit.test "error - unknown identifier"
    (Unit.assert-signal-name (string->tst "y") 'UnknownIdentifier)
)

(Unit.test "hello world"
    (const tst (string->tst "(println \"hello world\")"))

    (Unit.assert-equals (count tst) 1)
    (const tst' (car tst))

    (Unit.assert-truthy (CallPrintLn? tst'))
    (Unit.assert-equals (CallPrintLn-args tst') (list (StringLiteral "hello world")))
)

;; (Unit.test "declare constant"
;;     (Unit.assert-signal-name (string->tst "(const)") 'InvalidForm)
;;     (Unit.assert-signal-name (string->tst "(const x)") 'InvalidForm)
;;
;;     (string->tst "(const x 10)")
;; )
