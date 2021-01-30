(assoc! (car **root**) 
  'caar (fn (l) (car (car l)))
  'cadr (fn (l) (car (cdr l))))
