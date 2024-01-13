;; A small funciton to translate a subset
;; of lisp into ./al Alphabetical Lisp
(setq
 trans
 '(numberp "#" stringp "$" and & quote "'"
   atom "." atomp "." symbolp "."
   define ":" eq "=" is "?"
   return "^"
   car A member B cons C cdr D eval E
   format F if I
   consp K listp K sequencep K
   equal Q recurse R
   null U nilp U
   terpri T undefinedp U undefp U
   princ W prin1 X read Y apply Z
   floor _ or "|" not "~"
   ))

;; for now use l prefix
(setq
 trans
 '(numberp "l#" stringp "l$" and & quote "l'"
   atom "l." atomp "l." symbolp "l."
   define "l:" eq "l=" is "l?"
   return "^"
   car A member B cons C cdr D eval E
   format F if I
   consp K listp K sequencep K
   equal Q recurse R
   null U nilp U
   terpri T undefinedp U undefp U
   princ W prin1 X read Y apply Z
   floor _ or "l|" not "~"
   ))

(defun cal (x) 
  (cond 
   ((null x) (list "'nil"))
   ((numberp x) (list x " "))
   ((stringp x) (list "\"" x "\""))
   ((atom x) (list (or (cadr (member x trans)) x)))
   ((consp x)
    (cond
     ((eq (car x) 'quote)
      (append (list "'")
	      (cal (cadr x))
	      (list " ")))
     ((eq (car x) 'cond)
      (cal (cond2if x)))
     ((eq (car x) 'if)
      (append (cal (cadr x))
	      (cal (car x))
	      (append '("{") (cal (caddr x)) '("}"))
	      (append '("{") (cal (cadddr x)) '("}"))))
     (t (append (apply 'append (mapcar 'cal (cdr x))) (cal (car x))))))))

(defun condcases (x)
  (if (not (consp x)) x
    (if (not (consp (car x))) x
      (list 'if (caar x) (cadar x)
	    (condcases (cdr x))))))

(condcases '())
(condcases '(5))
(condcases '((5 7)))
(condcases '((5 7)(3 4)))


(pr (cal (condcases '((5 7)(3 4)))))

(pr (cal (condcases '((5 7)(3 4)))))


(defun cond2if (x) 
  (if (not (eq (car x) 'cond)) x
    (condcases (cdr x))))

(defun pr (x) 
  (apply 'concat
	 (mapcar
	  '(lambda(x) (format "%s" x))
	  x)))

(pr (cal '(if (null a) 0
	    (if (numberp a) a
	      (if (consp a)
		  (+ (recurse (car a))
		     (recurse (cdr a)))
		0)))))

(defun opt (x)
  (if (null x) x
    (cons
     (car x)
     (opt
      (if (and
	   (numberp (car x))
	   (equal (cadr x) " ")
	   (not (numberp (caddr x))))
	  (cddr x)
	(cdr x))))))

(defun al (x)
  (pr (opt (cal x))))

(al '(cond ((null a) 0)
	   ((numberp a) a)
	   ((consp a) 
	    (+ (recurse (car a))
	       (recurse (cdr a))))
	   (1 0)))

(member 'c '(a 1 b 2 c 3 d 4))
(c 3 d 4)

;; ???
;; cannot remove "} }" spaces as a missing
;; else clause could be concatted?
;; (that is if would be merged with loop)


;; or.....?
"aUI{0}{a#I{a}{aKI{aARaDR+}{1I{0}{'nil}}}}"
"aUI{0}{a#I{a}{aKI{aARaDR+}{1I{0}{'nil} } } } "

;; spaces edited...

;; handwritten!
;; dUI{\0}{dl#I{ }{d KI{d AR s DR+}   }}

;; generated
;;"aUI{ 0}{a #I{a}{a KI{a AR a DR+}{0}}}"


(pr (cal '(42 (quote 42) 42)))

(pr (cal "foo"))
(pr (cal '(quote foo)))

(pr (cal 42))
(pr (cal 'foo))
(pr (cal 'if))
(pr (cal 'cons))
(pr (cal '(+ 3 4)))
(pr (cal '(if (= 3 4) (+ 3 4) (- 3 4))))
(pr (cal '(42 'foo 42)))
(pr (cal '(return 33)))

;; no cond list mapcar append ...
(pr (cal (symbol-function 'cal)))
"txx listx#xtransBDAx|listx.atomlistxDAcalappendxA''=xDAcalxAcal'{xDDAcal'}append'{xDDDAcal'} appendappendxA'I='append'calxDmapcarZxAcalappendtcondxKcondclosure"






















