(import "./compiler.scm" :as Compiler)
(import "./llvm/ir/module.scm" :as Module)
(import "./parser.scm" :as Parser)
(import "./scanner.scm" :as Scanner)
(import "./static.scm" :as Static)

(const (write-file ir name)
    (*builtin*.write-file name (Module.module->string ir))
)

(const (compile program)
    (const input-file-name (str program ".mlsp"))
    (const ir ((|> *builtin*.slurp Scanner.string->scanner Parser.scanner->ast Static.ast->tst Compiler.tst->ir) input-file-name))
    
    (write-file ir (str program ".ll"))
    (println (Compiler.assemble (str program ".ll") (str program ".bc")))
    (println (Compiler.link (str program ".bc") program))
)

(compile "x")
