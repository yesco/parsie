(defun silly-loop (n)
  "Return the time, in seconds, to run N iterations of a loop."
  (let ((t1 (float-time)))
    (while (> (setq n (1- n)) 0))
    (- (float-time) t1)))

(silly-loop 50000000)
30.47821068763733

⇒ 5.200886011123657

(byte-compile 'silly-loop)
#[257 "\300 S\211\262\301V\204 \300 Z\207" [float-time 0] 4 "Return the time, in seconds, to run N iterations of a loop.

(fn N)"]

(silly-loop 50000000)
2.6944985389709473

⇒ 0.6239290237426758

(defun f (n)
  (if (= n 0) 0
    (if (= n 1) 1
      (+ (f (- n 1)) (f (- n 2))))))

(defun fib (n)
  (let ((t1 (float-time)))
    (f n)
    (- (float-time) t1)))

(fib 35)

;; byte compiled
;; 3.3613412380218506

;; recursive plain
;; 103.24632740020752

(byte-compile 'f)
#[257 "\211\300U\203 \300\207\211\301U\203 \301\207\302S!\302\303Z!\\\207" [0 1 f 2] 5 "

(disassemble (byte-compile 'f))
byte code:
  doc:   ...
  args: (arg1)
0	dup	  
1	constant  0
2	eqlsign	  
3	goto-if-nil 1
6	constant  0
7	return	  
8:1	dup	  
9	constant  1
10	eqlsign	  
11	goto-if-nil 2
14	constant  1
15	return	  
16:2	constant  f
17	stack-ref 1
18	sub1	  
19	call	  1
20	constant  f
21	stack-ref 2
22	constant  2
23	diff	  
24	call	  1
25	plus	  
26	return	  

;; more or less equivalent!!!
;; ALF - :Fd0=?{0^}{d1=?{1^}`11-Fx`12-Fx+^;

(fn N)"]

;; -*- lexical-binding: t; -*-
(defun foo (x y)
  (lambda () x))

((foo :bar :ignored))

(closure ((x . :bar)) nil x)

