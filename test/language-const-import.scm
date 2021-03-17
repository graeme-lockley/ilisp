(const (inc n) (plus n 1))

(const- (plus a b) (+ a b))

(let (dec n) ((*builtin*.atom-dereference minus) n 1))

(let- (minus a b) (- a b))
