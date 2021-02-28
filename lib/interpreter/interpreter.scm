(import "./environment.scm" :as Environment)
(import "./frame.scm" :as Frame)

(export (eval env exp)
    (if (self-evaluating? exp) 
            exp

        (variable? exp) 
            (Environment.lookup-variable-value env exp)

        (quoted? exp) 
            (text-of-quotation exp)

        (assignment? exp) 
            (eval-assignment env exp)

        (definition? exp) 
            (eval-definition env exp)

        (if? exp) 
            (eval-if env exp)

        (lambda? exp) 
            (make-procedure env (lambda-parameters exp) (lambda-body exp))

        (begin? exp)
            (eval-sequence env (begin-actions exp))

        (cond? exp) 
            (eval env (cond->if exp))

        (application? exp)
            (apply (eval env (operator exp)) (list-of-values env (operands exp)))

        (raise "Unknown expression type" {:exp exp :procedure 'eval})
    )
)

(define (apply procedure arguments) 
    (if (primitive-procedure? procedure)
            (apply-primitive-procedure procedure arguments)

        (compound-procedure? procedure)
            (eval-sequence
                (procedure-body procedure)
                (extend-environment
                    (procedure-parameters procedure)
                    arguments
                    (procedure-environment procedure)))

        (raise 'UnknownProcedureType {:procedure 'apply :procedure-type procedure})
    )
)

(define (list-of-values env exps) 
    (if (no-operands? exps)
        '()
        (cons
            (eval env (first-operand exps))
            (list-of-values env (rest-operands exps))
        )
    )
)

(define (eval-assignment env exp)
    (do
        (Environment.set-variable-value! 
            env
            (assignment-variable exp)
            (eval env (assignment-value exp))
        )

        ()
    )
)

(define (tagged-list? exp tag)
    (if (pair? exp)
            (= (car exp) tag)

        f
    )
)

(define (self-evaluating? exp)
    (or (number? exp) (string? exp))
)

(define (variable? exp)
    (symbol? exp)
)

(define (quoted? exp)
    (tagged-list? exp 'quote)
)

(define (text-of-quotation exp)
    (car (cdr exp))
)

(define (assignment? exp)
    (tagged-list? exp 'set!)
)

(define (assignment-variable exp) 
    (nth exp 1)
)

(define (assignment-value exp) 
    (nth exp 2)
)
