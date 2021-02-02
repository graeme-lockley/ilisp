(export (library-use library name)
    (if (contains? library name) 
        (get library name)
        (raise 'UnknownSymbolInLibrary {:name name})
    )
)

(export (library-import name)
    (do
        (define (load-library-file name)
            ((fn () 
                (do 
                    (define nested-nested-scope
                        ((fn () 
                            (do 
                                (eval (read-string (str "(do " (slurp name) "\n)") name))
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

        (if (not (contains? imports-list name)) 
            (do
                (assoc! imports-list name ())
                (define bindings (load-library-file name))
                (if (not (nil? (get imports-list name)))
                    (raise 'CyclicPackageDependency {:name name})
                )
                (assoc! imports-list name bindings)
            )
        )
    
        (get imports-list name)
    )
)


(export-macro (import name . options) 
    `(if (and (= ~(first options) :as) (not (nil? '~(nth options 1))))
        (define ~(nth options 1) (library-import ~name))
        (raise 'IllegalImportSyntax {:expected ":as symbol"})
    )
)
