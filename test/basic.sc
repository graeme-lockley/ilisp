(*builtin*.map-assoc! (*builtin*.pair-car **root**) 
  'caar (proc (l) (*builtin*.pair-car (*builtin*.pair-car l)))
  'cadr (proc (l) (*builtin*.pair-car (*builtin*.pair-cdr l))))
