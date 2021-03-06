(const (module-use module name)
    (if (*builtin*.map-contains? module name) 
        (map-get module name)
        (raise 'UnknownSymbolInModule {:name name})
    )
)

(macro (module-import name)
    `(*builtin*.import-source ~name)
)

(macro (import name . options) 
    (if (and (= (first options) :as) (not (*builtin*.null? ((map-get *builtin* 'list-nth) options 1))))
            `(const- ~((map-get *builtin* 'list-nth) options 1) (module-import ~name))
        (= (first options) :names)
            (do
                (const imports (*builtin*.list-map (rest options) (proc (n) `(const- ~n (map-get (module-import ~name) (quote ~n))))))
                `(do ~@imports)
            )
        (raise 'IllegalImportSyntax {:expected ":as or :names symbol"})
    )
)
