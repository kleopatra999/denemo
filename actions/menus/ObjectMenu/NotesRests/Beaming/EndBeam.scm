;;; EndBeam
(let ((tag "Beam"))
	(if (d-Directive-chord? tag)
		(d-DirectiveDelete-chord tag)
		(begin
			(d-DirectivePut-chord-postfix tag "]")
			(d-DirectivePut-chord-display tag  "]")
			(d-RefreshDisplay)
			(d-SetSaved #f))))
