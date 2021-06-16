(import "./compiler.scm" :as Compiler)
(import "./llvm/ir/module.scm" :as Module)
(import "./parser.scm" :as Parser)
(import "./scanner.scm" :as Scanner)
(import "./static.scm" :as Static)

(const (write-file ir name)
    (*builtin*.write-file name (Module.module->string ir))
)

(const (compile program ilib-file-name)
    (const input-file-name (str program ".mlsp"))
    (const ir ((|> *builtin*.slurp Scanner.string->scanner Parser.scanner->ast Static.ast->tst Compiler.tst->ir) input-file-name))
    
    (write-file ir (str program ".ll"))
    (println (Compiler.assemble (str program ".ll") (str program ".bc")))
    (println (Compiler.link (str program ".bc") ilib-file-name program))
)

(const ilib-file-name
    (try
        *env*.MINI_ILISP_LIB
        (proc (s) "lib.bc" )
    )
)

(if (not (= 1 (count *args*)))
        (println "Error: incorrect number of arguments")
    (compile (car *args*) ilib-file-name)
)
