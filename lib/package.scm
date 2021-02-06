(export (package-use package name)
    (if (contains? package name) 
        (get package name)
        (raise 'UnknownSymbolInPackage {:name name})
    )
)

(export (package-import-with-context name *source-name*)
    (try
        (do
            (define *source-name* ((get (car **root**) :builtins 'file-name-relative-to-file-name) *source-name* name))

            (define (load-package-file)
                ((fn () 
                    (do 
                        (define nested-nested-scope
                            ((fn () 
                                (do 
                                    (eval (read-string (str "(do " (slurp *source-name*) "\n)") *source-name*))
                                    (cdr **scope**)
                                )
                            ))
                        )
                        (dissoc (car nested-nested-scope) 'nested-nested-scope)
                    )
                ))
            )

            (define root-scope (car **root**))

            (define imports-list
                (do
                    (if (not (contains? root-scope :imports))
                        (assoc! root-scope :imports {}))
                    (get root-scope :imports)
                )
            )

            (if (not (map? (get imports-list *source-name*)))
                (do
                    (println "Loading " *source-name*)
                    (assoc! imports-list *source-name* ())
                    (define bindings (load-package-file))
                    (if (map? (get imports-list *source-name*))
                        (raise 'CyclicPackageDependency {:name *source-name*})
                    )
                    (assoc! imports-list *source-name* bindings)
                )
            )
        
            (get imports-list *source-name*)
        ) 
        (fn (e)
            (do
                (println "error loading " *source-name* ": " e)
                (raise e)
            )
        )
    )
)


(export-macro (package-import name)
    `(package-import-with-context ~name *source-name*)
)

(export-macro (import name . options) 
    `(if (and (= ~(first options) :as) (not (nil? '~(nth options 1))))
        (define ~(nth options 1) (package-import ~name))
        (raise 'IllegalImportSyntax {:expected ":as symbol"})
    )
)
