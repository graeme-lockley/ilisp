(const- *verbose* #f)

(const (verbose?) *verbose*)

(const (verbose v) (set! *verbose* v))

(const (package-use package name)
    (if (contains? package name) 
        (get package name)
        (raise 'UnknownSymbolInPackage {:name name})
    )
)

(const (package-import-with-context name *source-name*)
    (try
        (do (set! *source-name* (*builtin*.file-name-relative-to-file-name *source-name* name))

            (const (load-package-file)
                ((proc () 
                    (const nested-nested-scope
                        ((proc ()
                            (const *top-level* :t) 
                            (eval (read-string (str "(do " (slurp *source-name*) "\n)") *source-name*))
                            (cdr **scope**)
                        ))
                    )
                    (dissoc (car nested-nested-scope) 'nested-nested-scope)
                ))
            )

            (const root-scope (car **root**))

            (const imports-list
                (do (if (not (contains? root-scope :imports))
                        (assoc! root-scope :imports {}))
                    (get root-scope :imports)
                )
            )

            (if (not (map? (get imports-list *source-name*)))
                (do (if *verbose* (println "Loading " *source-name*))
                    (assoc! imports-list *source-name* ())
                    (const bindings (load-package-file))
                    (if (map? (get imports-list *source-name*))
                        (raise 'CyclicPackageDependency {:name *source-name*})
                    )
                    (assoc! imports-list *source-name* bindings)
                )
            )
        
            (get imports-list *source-name*)
        )
        (proc (e)
            (if *verbose* (println "error loading " *source-name* ": " e))
            (raise e)
        )
    )
)

(macro (package-import name)
    `(package-import-with-context ~name *source-name*)
)

(macro (import name . options) 
    (if (and (= (first options) :as) (not (nil? ((get *builtin* 'list-nth) options 1))))
        `(const ~((get *builtin* 'list-nth) options 1) (package-import ~name))
        (raise 'IllegalImportSyntax {:expected ":as symbol"})
    )
)
