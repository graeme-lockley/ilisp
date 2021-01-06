# TODO

## Build an iLisp- interpreter entirely in C

- Implement Value
   - VT_CHARACTER
   - VT_NUMBER
   - VT_STRING
   - VT_PAIR
   - VT_VECTOR
   - VT_MAP
   - VT_NATIVE_FUNCTION
   - VT_FUNCTION

- Implement different forms of toString which converts a Value into a string
   - VT_NIL
   - VT_SYMBOL   
   - VT_KEYWORD
   - VT_CHARACTER
   - VT_NUMBER
   - VT_STRING
   - VT_PAIR
   - VT_VECTOR
   - VT_MAP
   - VT_NATIVE_FUNCTION
   - VT_FUNCTION

- Implement core functions:
   - symbol?
   - keyword?
   - character?
   - number?
   - string?
   - pair?
   - vector?
   - map?
   - function?
   - string->symbol
   - symbol->string
   - string->keyword
   - keyword->string