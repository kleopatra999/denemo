;;; BeamRightTwo
(let ((tag "BeamRight"))
(if (d-Directive-chord? tag)
	(d-DirectiveDelete-chord tag)
	(begin
	(d-DirectivePut-chord-prefix tag "\\set stemRightBeamCount = #2 ")
	(d-DirectivePut-chord-override tag DENEMO_OVERRIDE_AFFIX)
	(d-DirectivePut-chord-display tag  "[2")))
(d-RefreshDisplay)
(d-SetSaved #f))
