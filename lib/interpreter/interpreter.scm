(import "./environment.scm" :as Environment)
(import "./frame.scm" :as Frame)

(define builtin-apply apply)

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

        (do? exp)
            (eval-sequence env (do-actions exp))

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
                (Environment.extend
                    (procedure-environment procedure)
                    (procedure-parameters procedure)
                    arguments
                )
                (procedure-body procedure)
            )

        (raise 'UnknownProcedureType {:procedure 'apply :procedure-type procedure})
    )
)

(define (apply-primitive-procedure procedure arguments)
    (builtin-apply (primitive-implementation procedure) arguments)    
)

(define (list-of-values env exps) 
    (if (no-operands? exps)
        ()
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

(define (eval-definition env exp)
    (do
        (Environment.define-variable! env (definition-variable exp) (eval env (definition-value exp)))

        ()
    )
)

(define (eval-if env exp)
    (if (true? (eval env (if-predicate exp)))
        (eval env (if-consequant exp))
        (eval env (if-alternative exp))
    )
)

(define (eval-sequence env exps)
    (if (do-actions-empty? exps)
            ()

        (do-actions-last? exps)
            (eval env (do-actions-first exps))

        (do
            (eval env (do-actions-first exps))
            (eval-sequence env (do-actions-rest exps))
        )
    )
)

(define (make-lambda parameters body) 
    (cons 'lambda (cons parameters body))
)

(define (make-if predicate consequent alternative)
    (list 'if predicate consequent alternative)
)

(define (tagged-list? exp tag)
    (if (pair? exp)
            (= (car exp) tag)

        f
    )
)

(define (self-evaluating? exp)
    (or (number? exp) (string? exp) (nil? exp))
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

(define (definition? exp)
    (tagged-list? exp 'define)
)

(define (definition-variable exp)
    (if (symbol? (nth exp 1))
          (nth exp 1)
        (car (nth exp 1))
    )
)

(define (definition-value exp)
    (if (symbol? (nth exp 1))
            (nth exp 2)
        (make-lambda (cdr (nth exp 1)) (drop exp 2))
    )
)

(define (if? exp)
    (tagged-list? exp 'if)
)

(define (if-predicate exp)
    (nth exp 1)
)

(define (if-consequant exp)
    (nth exp 2)
)

(define (if-alternative exp)
    (nth exp 3)
)

(define (true? exp)
    exp
)

(define (lambda? exp)
    (tagged-list? exp 'lambda)
)

(define (lambda-parameters exp)
    (nth exp 1)
)

(define (lambda-body exp)
    (drop exp 2)
)

(define (do? exp)
    (tagged-list? exp 'do)
)

(define (do-actions exp)
    (cdr exp)
)

(define (do-actions-empty? exps) (nil? exps))
(define (do-actions-last? exps) (nil? (cdr exps)))
(define (do-actions-first exps) (car exps))
(define (do-actions-rest exps) (cdr exps))

(define (application? exp)
    (pair? exp)
)

(define (operator exp) 
    (car exp)
)

(define (operands exp) 
    (cdr exp)
)

(define (no-operands? ops) 
    (nil? ops)
)

(define (first-operand ops)
    (car ops)
)

(define (rest-operands ops)
    (cdr ops)
)

(define (operator exp)
    (car exp)
)

(define (make-procedure env parameters body)
    (list 'procedure parameters body env)
)

(define (primitive-procedure? exp) 
    (tagged-list? exp 'primitive)
)

(define (compound-procedure? exp)
    (tagged-list? exp 'procedure)
)

(define (primitive-implementation proc) 
    (car (cdr proc))
)

(define (procedure-parameters p)
    (nth p 1)
)

(define (procedure-body p)
    (nth p 2)
)

(define (procedure-environment p)
    (nth p 3)
)
