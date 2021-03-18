(import "./environment.scm" :as Environment)
(import "./frame.scm" :as Frame)

(const- builtin-apply apply)

(const (eval env exp)
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

(const- (apply procedure arguments)
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

(const- (apply-primitive-procedure procedure arguments)
    (builtin-apply (primitive-implementation procedure) arguments)    
)

(const- (list-of-values env exps) 
    (if (no-operands? exps)
        ()
        (cons
            (eval env (first-operand exps))
            (list-of-values env (rest-operands exps))
        )
    )
)

(const- (eval-assignment env exp)
    (do
        (Environment.set-variable-value! 
            env
            (assignment-variable exp)
            (eval env (assignment-value exp))
        )

        ()
    )
)

(const- (eval-definition env exp)
    (do
        (Environment.define-variable! env (definition-variable exp) (eval env (definition-value exp)))

        ()
    )
)

(const- (eval-if env exp)
    (if (true? (eval env (if-predicate exp)))
        (eval env (if-consequant exp))
        (eval env (if-alternative exp))
    )
)

(const- (eval-sequence env exps)
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

(const- (make-lambda parameters body) 
    (cons 'lambda (cons parameters body))
)

(const- (make-if predicate consequent alternative)
    (list 'if predicate consequent alternative)
)

(const- (tagged-list? exp tag)
    (if (pair? exp)
            (= (car exp) tag)

        #f
    )
)

(const- (self-evaluating? exp)
    (or (number? exp) (string? exp) (nil? exp) (boolean? exp))
)

(const- (variable? exp)
    (symbol? exp)
)

(const- (quoted? exp)
    (tagged-list? exp 'quote)
)

(const- (text-of-quotation exp)
    (car (cdr exp))
)

(const- (assignment? exp)
    (tagged-list? exp 'set!)
)

(const- (assignment-variable exp) 
    (nth exp 1)
)

(const- (assignment-value exp) 
    (nth exp 2)
)

(const- (definition? exp)
    (tagged-list? exp 'define)
)

(const- (definition-variable exp)
    (if (symbol? (nth exp 1))
          (nth exp 1)
        (car (nth exp 1))
    )
)

(const- (definition-value exp)
    (if (symbol? (nth exp 1))
            (nth exp 2)
        (make-lambda (cdr (nth exp 1)) (drop exp 2))
    )
)

(const- (if? exp)
    (tagged-list? exp 'if)
)

(const- (if-predicate exp)
    (nth exp 1)
)

(const- (if-consequant exp)
    (nth exp 2)
)

(const- (if-alternative exp)
    (nth exp 3)
)

(const- (true? exp)
    exp
)

(const- (lambda? exp)
    (tagged-list? exp 'lambda)
)

(const- (lambda-parameters exp)
    (nth exp 1)
)

(const- (lambda-body exp)
    (drop exp 2)
)

(const- (do? exp)
    (tagged-list? exp 'do)
)

(const- (do-actions exp)
    (cdr exp)
)

(const- (do-actions-empty? exps) (nil? exps))
(const- (do-actions-last? exps) (nil? (cdr exps)))
(const- (do-actions-first exps) (car exps))
(const- (do-actions-rest exps) (cdr exps))

(const- (application? exp)
    (pair? exp)
)

(const- (operator exp) 
    (car exp)
)

(const- (operands exp) 
    (cdr exp)
)

(const- (no-operands? ops) 
    (nil? ops)
)

(const- (first-operand ops)
    (car ops)
)

(const- (rest-operands ops)
    (cdr ops)
)

(const- (make-procedure env parameters body)
    (list 'procedure parameters body env)
)

(const- (primitive-procedure? exp) 
    (tagged-list? exp 'primitive)
)

(const- (compound-procedure? exp)
    (tagged-list? exp 'procedure)
)

(const- (primitive-implementation proc) 
    (car (cdr proc))
)

(const- (procedure-parameters p)
    (nth p 1)
)

(const- (procedure-body p)
    (nth p 2)
)

(const- (procedure-environment p)
    (nth p 3)
)
