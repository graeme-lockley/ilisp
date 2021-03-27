(*builtin*.map-assoc! (car **root**) 
  'caar (proc (l) (car (car l)))
  'cadr (proc (l) (car (cdr l))))
