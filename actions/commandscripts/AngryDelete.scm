;;; Warning!!! This file is derived from those in actions/menus/... do not edit here
;;; Angry Delete filter
(let ((midi 0)
      (command 0)
      (note 0)
      (velocity 0)
      (loop 0))
     (d-InputFilterNames (_ "Angry Delete MIDI Filter"))
     (d-SetMidiCapture #t)
     (set! loop  (lambda ()
		(begin		  
		  (set! midi (d-GetMidi))
		  (set! velocity (bit-extract midi 16 24))
		  (set! command (bit-extract midi 0 8))
		  
		  (if (and (= command #x90)(> velocity 90))
		       (begin 
		       		(d-PlayMidiKey #xF06001)
		       		(if (d-GetNonprinting)
                              		(d-PrevNote)
                              		(d-DeletePreviousObject))))
		  (d-PutMidi midi)
		  (if (= command 0)
		      (display "Filter stopping")
		      (loop)))))
     (loop))
(d-SetMidiCapture #f)
