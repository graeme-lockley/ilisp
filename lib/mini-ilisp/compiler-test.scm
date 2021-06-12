(import "../unit.scm" :as Unit)

(import "./compiler.scm" :as Compiler)
(import "./llvm/ir/module.scm" :as Module)
(import "./parser.scm" :as Parser)
(import "./scanner.scm" :as Scanner)
(import "./static.scm" :as Static)

(const (compile-and-run program)
    (const ir ((|> Scanner.string->scanner Parser.scanner->ast Static.ast->tst Compiler.tst->ir) program))
    (write-file ir "test.ll")
    (Compiler.assemble "test.ll" "test.bc")
    (Compiler.run "test.bc")
)

(const (write-file ir name)
    (*builtin*.write-file name (Module.module->string ir))
)

(Unit.test "hello world"
    (Unit.assert-equals (compile-and-run "(println \"hello world\")") "hello world\n")
    (Unit.assert-equals (compile-and-run "(println \"hello\" \" \" \"world\")") "hello world\n")
    (Unit.assert-equals (compile-and-run "(println \"hello\" \" \" \"world\") (println \"bye bye love\")") "hello world\nbye bye love\n")
)

(Unit.test "literal - integer"
    (Unit.assert-equals (compile-and-run "(print 123 \" \" -456)") "123 -456")
)

(Unit.test "literal - boolean"
    (Unit.assert-equals (compile-and-run "(print #t \" \" #f)") "#t #f")
)

(Unit.test "operators - +"
    (Unit.assert-equals 
        (compile-and-run "(print (+) \" \" (+ 1) \" \" (+ 1 2) \" \" (+ 1 2 3 4 5 6))")
        "0 1 3 21"
    )
)
