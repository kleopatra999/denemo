;;;StartTrillSpan
(if (not (Note?))
  (d-MoveCursorLeft))
 (if (Note?)
	(let ((tag "StartTrillSpan"))
	(d-MoveCursorRight)
	 (if (d-Directive-standalone? tag)
	    (let ((choice (RadioBoxMenu (cons (_ "Help") 'help)(cons (_ "No \"tr\" at start.") 'start) (cons (_ "Delete") 'delete))))  
		(case choice
		    ((help)
		        (d-InfoDialog (_ "This marks the start of a trill extending over several notes. The leading \"tr\" can be omitted if desired.")))
		    ((start)
		        (d-DirectivePut-standalone-prefix tag "-\\tweak bound-details.left.text #'() "))
		    ((delete)
		        (d-DirectiveDelete-standalone tag))))
	    (begin
		(if (d-MoveCursorLeft)
		    (if (d-Directive-standalone? tag)
		      (d-DirectiveDelete-standalone tag)
		        (d-MoveCursorRight)))
	    (StandAloneDirectiveProto (cons "StartTrillSpan" "\\startTrillSpan") #f LG-Prall)
	    (d-DirectivePut-standalone-gx tag 10)
	    (d-DirectivePut-standalone-grob tag tag)
	    (d-MoveCursorRight)))
	(d-RefreshDisplay)
	(d-SetSaved #f))
(d-WarningDialog (_ "Cursor must be on the note where the trill is to start")))
        
