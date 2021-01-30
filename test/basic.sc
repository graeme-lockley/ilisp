(assoc! (car **root**) 'caar (fn (l) (car (car l))))

(assoc! (car **root**) 'cadr (fn (l) (car (cdr l))))
