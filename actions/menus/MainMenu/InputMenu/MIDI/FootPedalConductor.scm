;;;;;;;;;;FootPedalConductor
(define (FootPedalConductor::display text)
  (if (positive? (d-DirectiveGet-standalone-gy "Conductor"))
  (begin
    (d-DirectivePut-standalone-graphic "Conductor" (string-append "\n" text "\nDenemo\n620"))
    (d-RefreshDisplay))))
  
(let ((start #f) (number #f) (numerator (string->number (car (string-split (d-InsertTimeSig "query=timesigname") #\/)))))
(define (pedalDown? midi)
    (and (= #xB0 (bit-extract midi 0 8))  (= (bit-extract midi 8 16) #x40) (=  (bit-extract midi 16 24) #x7F)))
(define (pedalUp? midi)
    (and (= #xB0 (bit-extract midi 0 8))  (= (bit-extract midi 8 16) #x40) (=  (bit-extract midi 16 24) #x0)))
(define (thetime) (/ (get-internal-real-time) internal-time-units-per-second)) ;;; time in seconds
(d-SetBackground #xB0B0E0)
(d-DirectivePut-standalone-gy "Conductor" 480)
(d-MoveCursorLeft)
(let loop ((number -1))
 (FootPedalConductor::display (string-append (number->string number) " ⬇"))
 (let ((midi (d-GetMidi)))  
 (disp midi)
    (if (pedalDown? midi)
	(begin
	 (if start
	    (let* ((now (thetime)) (interval (- now start)))
	      (if (> interval (/ 1 2))
		(begin
		  (set! start now)
		    (if (> number -1)
		      (let ( (at #f) (text (string-append (number->string (+ 1 number)) " " "⬆")))
		      (cond ((or (= numerator 2) (= numerator 6))
			      (set! text (string-append (number->string (+ 1 number)) " " "⬆"))
			      (set! at (round (* interval 500)))
			      (d-OneShotTimer at (string-append "(FootPedalConductor::display \"" text "\")")))
			    ((or (= numerator 3) (= numerator 9))
				(set! text (string-append (number->string (+ 1 number)) " " " ➨"))
				(set! at (round (* interval 333)))
				(d-OneShotTimer at (string-append "(FootPedalConductor::display \"" text "\")"))
				(set! text (string-append (number->string (+ 1 number)) " " " ⇖"))
				(set! at (round (* interval 666)))
				(d-OneShotTimer at (string-append "(FootPedalConductor::display \"" text "\")")))

			    ((or (= numerator 4) (= numerator 12))
			      (set! text (string-append (number->string (+ 1 number)) " " "↩"))
			      (set! at (round (* interval 250)))
			      (d-OneShotTimer at (string-append "(FootPedalConductor::display \"" text "\")"))
			      (set! text (string-append (number->string (+ 1 number)) " " " ➨"))
			      (set! at (round (* interval 500)))
			      (d-OneShotTimer at (string-append "(FootPedalConductor::display \"" text "\")"))
			      (set! text (string-append (number->string (+ 1 number)) " " "⇖"))
			      (set! at (round (* interval 750)))
			      (d-OneShotTimer at (string-append "(FootPedalConductor::display \"" text "\")"))
			      ))))
		  (loop (+ 1 number)))
		  (disp "finished")))
	    (begin ;;; not started yet
	    	(set! start (thetime))
	    	(loop (+ 1 number)))))
	  (begin
	    (if (pedalUp? midi)
	    (loop number)))))))
(d-SetBackground #xFFFFFF)
(d-DirectiveDelete-standalone "Conductor")