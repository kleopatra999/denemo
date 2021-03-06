;;;Tremolo
(let ((command #f) (duration #f) (dur #f)(stripes #f))
(set! command (lambda ()
		 (d-WarningDialog (_ "To use this function correctly you need to press a keyboard shortcut for a duration command\nThe duration will be used to divide the note at the cursor when printed. Durations 3-7 are valid (that is eighth note and shorter)"))))
(if (d-Directive-chord? "Tremolo")
	(d-DirectiveDelete-chord "Tremolo")
	(begin 
		(if (not DenemoKeypressActivatedCommand)
 			(d-InfoDialog (_ "Click on the display and press a shortcut key 3-7 for the duration into which the note at the cursor is to be divided")))
   	  (set! duration (d-GetCommand))
          (set! dur #f)
   	  (cond
		 ((equal? duration "d-3") (set! dur ":8")(set! stripes "𝅪"))
		 ((equal? duration "d-4") (set! dur ":16")(set! stripes "𝅫"))
		 ((equal? duration "d-5") (set! dur ":32")(set! stripes "𝅬"))
		 ((equal? duration "d-6") (set! dur ":64")(set! stripes "𝅪𝅫"))
		 ((equal? duration "d-7") (set! dur ":128")(set! stripes "𝅫𝅫"))
 	     )
          (if (string? dur)
         	 (begin		
          		(d-DirectivePut-chord-display "Tremolo" stripes)
   			(d-DirectivePut-chord-postfix "Tremolo" dur))
	 	 (command))
(d-SetSaved #f)
(d-RefreshDisplay))))