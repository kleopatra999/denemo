 ;; ;;;;;;;;;;;;MetronomeMarking
 (let ((input "") (len 1) (dotted #f)(duration "4")(bpm 60)(midiBpm 60)(valid #f) )
(set! input (d-GetUserInput "Metronome Marking" "Give unit beat duration \n(e.g., 4. for dotted-quarter):" "4" ))
(set! len (string-length input) )
(set! dotted (equal? "." (substring input (- len 1) len )) ) ;see if a dot at end
(if (equal? dotted #t)
(set! duration (substring input 0 (- len 1))) ;if there's a dot, cut it off from input to get base duration.
(set! duration input) )
(set! bpm (d-GetUserInput "Metronome Marking"
"Give number of these beats per minute:" "60" ) )
(set! valid (not (equal? (and (string->number duration) (string->number bpm) ) #f))) ;don't go unless both are numbers.
;don't go unless base duration is valid lilypond: (could go higher if wanted):
(if (and (equal? valid #t) (or (equal? duration "1")(equal? duration "2")(equal? duration "4")(equal? duration "8")(equal? duration "16")) )
(begin
;want * 3/2 for dotted,*4 since midi uses quarters and divide by duration,
(if (equal? dotted #t) (set! midiBpm (number->string (floor (* (/ (string->number bpm) (string->number duration)) 6 ) ) ) )
(set! midiBpm (number->string (floor (* (/ (string->number bpm) (string->number duration) ) 4)) ) )
)
(d-Directive-standalone "MM")
(d-DirectivePut-standalone-prefix "MM" (string-append "\\override Score.MetronomeMark #'padding = #3
\\tempo " input " = " bpm))
(d-DirectivePut-standalone-override "MM" (logior DENEMO_OVERRIDE_TEMPO DENEMO_OVERRIDE_STEP))
(d-DirectivePut-standalone-midibytes "MM" midiBpm)
(d-DirectivePut-standalone-display "MM" (string-append input "=" bpm))
(d-DirectivePut-standalone-ty "MM" -5)
(d-DirectivePut-standalone-minpixels "MM" 15)
)
(d-WarningDialog "Incorrect syntax.")
))