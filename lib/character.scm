(const (whitespace? c) (<= c #x20))

(const (not-whitespace? c) (not (whitespace? c)))

(const ->character *builtin*.integer->character)