(import "./environment.scm" :as Environment)
(import "./frame.scm" :as Frame)

(export (eval exp env)
    (if (self-evaluating? exp) 
            exp

        (variable? exp) 
            (Environment.lookup-variable-value exp env)

        (quoted? exp) 
            (text-of-quotation exp)

        (assignment? exp) 
            (eval-assignment exp env)

        (definition? exp) 
            (eval-definition exp env)

        (if? exp) 
            (eval-if exp env)

        (lambda? exp) 
            (make-procedure (lambda-parameters exp) (lambda-body exp) env)

        (begin? exp)
            (eval-sequence (begin-actions exp) env)

        (cond? exp) 
            (eval (cond->if exp) env)

        (application? exp)
            (apply (eval (operator exp) env) (list-of-values (operands exp) env))

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

(define (list-of-values exps env) 
    (if (no-operands? exps)
        '()
        (cons
            (eval (first-operand exps) env)
            (list-of-values (rest-operands exps) env)
        )
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