;;; Warning!!! This file is derived from those in actions/menus/... do not edit here
;PrintScoreAndParts
(let ((initial #f))
(d-PushPosition)
(while (d-PreviousMovement))
(d-SelectDefaultLayout)
(set! initial (d-Directive-scoreheader? "BookTitle"))
(if (and initial (not (d-Directive-scoreheader? "BookInstrumentation")))
        (d-BookInstrumentation (_ "Full Score")))
(d-ReduceLayoutToLilyPond)       
(set! initial (d-DirectiveGet-scoreheader-display "BookInstrumentation"))
(while (d-MoveToStaffUp))
(let loop1 ()
	(if (not (d-DirectiveGet-staff-display "InstrumentName"))
		(d-InstrumentName))
	(if (d-MoveToStaffDown)
		(loop1)))
(let loop2 ()       
	(let ((thename (scheme-escape (GetNthLine (d-DirectiveGet-staff-display "InstrumentName") 0))))
		(if initial
			(d-BookInstrumentation thename))
		(d-LilyPondForPart)
		(d-ReduceLayoutToLilyPond);this fixes the instrumentation for this layout
		)		
	(if (d-MoveToStaffUp)
			(loop2)))
(d-PrintAllLayouts) ;; this works asynchronously
(if initial
	(d-BookInstrumentation initial))
(d-PopPosition))