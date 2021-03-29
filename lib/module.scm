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
        (raise 'IllegalImportSyntax {:expected ":as symbol"})
    )
)
