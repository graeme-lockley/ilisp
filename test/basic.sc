(map-set! (car **root**) 'caar (fn (l) (car (car l))))

(map-set! (car **root**) 'cadr (fn (l) (car (cdr l))))
