(import "./string-kata.scm" :as SK)

(Unit.test "given a blank should return 0"
    (Unit.assert-equals (SK.add "") 0)
)

(Unit.test "given a number should return it's value if less then 1001 else 9"
    (Unit.assert-equals (SK.add "123") 123)
    (Unit.assert-equals (SK.add "1001") 0)
)

(Unit.test "given numbers separated with a comma should return the sum of all less than 1001"
    (Unit.assert-equals (SK.add "1,2,10012,3") 6)
)

(Unit.test "given numbers separated with a comma or newline should return the sum of all less than 1001"
    (Unit.assert-equals (SK.add "1\n2,3") 6)
)

(Unit.test "given numbers separated with a single character custom separator should return the sum of all less than 1001"
    (Unit.assert-equals (SK.add "//;\n1;2;2001;3") 6)
    (Unit.assert-equals (SK.add "//*\n1*2*3*3001*4") 10)
)

(Unit.test "given numbers separated with a single multi-character custom separator should return the sum of all less than 1001"
    (Unit.assert-equals (SK.add "//[***]\n1***2***2001***3") 6)
)

(Unit.test "given numbers separated with multiple multi-character custom separators should return the sum of all less than 1001"
    (Unit.assert-equals (SK.add "//[***][===]\n1***2===2001***3") 6)
)

(Unit.test "given numbers with at least one negative should report an error with all of the negatives"
    (Unit.assert-signal (SK.add "0,-1,2,-3,4,-5,6")
        (fn (n)
            (Unit.assert-equals n (list (- 1) (- 3) (- 5)))
        )
    )
)
