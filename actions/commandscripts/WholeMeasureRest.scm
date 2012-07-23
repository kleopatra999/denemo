 ;;;WholeMeasureRest
 (let ((timesig #f))
(set! timesig (d-InsertTimeSig "query=timesigname")) 
(if (not (d-Directive-chord? "WholeMeasureRest"))
 	(d-InsertWholeRest))
 (d-SetDurationInTicks (* 1536 (GetPrevailingTimeSig #t)))
(d-DirectivePut-chord-graphic "WholeMeasureRest" "
\x20")
 (d-DirectivePut-chord-gx "WholeMeasureRest" 60)
 (d-DirectivePut-chord-display "WholeMeasureRest" "1")
 (d-DirectivePut-chord-tx "WholeMeasureRest" 76)
  (d-DirectivePut-chord-ty"WholeMeasureRest" 15)

(d-DirectivePut-chord-override "WholeMeasureRest" (logior DENEMO_OVERRIDE_LILYPOND DENEMO_OVERRIDE_GRAPHIC DENEMO_ALT_OVERRIDE))
(d-DirectivePut-chord-postfix "WholeMeasureRest" (string-append "R1*" timesig))
(d-SetSaved #f))