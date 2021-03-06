 ;;;;;;;;;;; TransposeStaffPrint

(define-once Transpose::Interval "c ees")

(let ((lily #f) (text #f)(params TransposeStaffPrint::params))
	(if (and params  (not (equal? params "Edit")))
		(set!  Transpose::Interval params)
      	 	(set! Transpose::Interval (d-GetUserInput (_ "Set Transpose Interval") (_ "Give Interval to transpose by
e.g. c ees means up minor third.
es = flat, so e.g. bes means b-flat
is = sharp so e.g fis means f-sharp
Use commas for octave(s) down, 
single-quotes for octave(s) up
e.g. c c' means octave up.
") Transpose::Interval)))
  (set! lily (string-append  "\\transpose " Transpose::Interval " "))
  (set! text (string-append  (_ "Print transposed:  ") Transpose::Interval " "))
 (d-PushPosition)
 (while (d-MoveToVoiceUp))
 (let loop ()
 	(d-DirectivePut-voice-override  "TransposeStaffPrint" DENEMO_OVERRIDE_GRAPHIC)
  	(d-DirectivePut-voice-display  "TransposeStaffPrint" text)
  	(d-DirectivePut-voice-prefix  "TransposeStaffPrint" lily)
 	 (if (d-MoveToVoiceDown)
 		 (loop)))
  (d-PopPosition)
  (d-SetSaved #f)
  (d-RefreshDisplay))
