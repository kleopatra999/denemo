;;;;;;PageBreak
(if (d-Directive-standalone?  "PageBreak")
	(d-InfoDialog "This sign denotes a Page Break. The music after this point will start on new page. This page break must be on a bar line. Use the Conditional Directives menu to make the page break applicable only to specific layouts. Delete using Del or Backspace key.")
	(begin
		(if (d-IsVoice)
			(begin
				(d-WarningDialog "The cursor is on a voice. Put line breaks in the main staff that the voice belongs to."))

			(let ((choice #f))
				(d-DirectivePut-standalone "PageBreak")
				(d-DirectivePut-standalone-postfix "PageBreak" "\\pageBreak")
				(d-DirectivePut-standalone-minpixels "PageBreak" 50)
				(d-DirectivePut-standalone-gy "PageBreak" -25)
				(d-DirectivePut-standalone-graphic "PageBreak" "
⁋
Denemo
26")
				(d-DirectivePut-standalone-minpixels "PageBreak" 10)
				(d-RefreshDisplay)
				(SetDirectiveConditional)
				(d-SetSaved #f)))))
