;;;Footnote
(let ((tag "Footnote"))
	(define (set-footnote mark text)
		(d-Chordize #t)
		
		(d-DirectivePut-chord-prefix tag "\\override Score.FootnoteItem #'annotation-line = ##f ")
		(d-DirectivePut-chord-override tag DENEMO_OVERRIDE_AFFIX)
		(d-DirectivePut-note-prefix tag (string-append "\\footnote \"" mark "\" #'(0 . 3.5) \\markup { \\super \"" mark "\" \\teeny \"" text "\"} "))
		(d-DirectivePut-note-display tag (string-append (_ "Fn") "\n" mark "\n" text))
		(d-DirectivePut-note-ty tag -30) 
		(d-RefreshDisplay)
		(d-SetSaved #f))
	(define (choose-footnote mark text)
			(set! mark (d-GetUserInput (_ "Footnote") (_ "Give footnote marker") mark))
			(set! text (d-GetUserInput  (_ "Footnote") (_ "Give footnote text") text))
			(if (and mark text)
			(set-footnote mark text)))
	(if (Appending?)
		(d-MoveCursorLeft))
	(if (or (Chord?) (Note?))
		(if (d-Directive-note? tag)
			(let ((choice #f) (current (d-DirectiveGet-note-display tag)))
				(define mark (GetNthLine current 1))
				(define text (GetNthLine current 2))
				;;; menu here to offer d-DirectiveTextEdit tag 
				(disp "About to ask for choice " choice " at start")
				(set! choice (GetEditOption))
				(disp "Now got choice " choice " ok\n")
				(case choice
					((edit) (choose-footnote mark text))
					((delete) (begin (d-DirectiveDelete-chord tag)(d-DirectiveDelete-note tag)))
					((advanced) (d-DirectiveTextEdit-note tag))
					(else (disp "A problem with eqv ..."))))
			(let ()
				(choose-footnote "*"(_ "Orig. "))))
			(d-InfoDialog (_ "No note or chord at cursor to attach footnote to"))))