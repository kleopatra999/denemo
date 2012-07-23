;;TocTitle
(d-LilyPondInclude "book-titling.ily")
(d-PushPosition)
(while (d-PreviousMovement))
(let ((tag "TableOfContents") (title "Contents") (pb 3) (mc  " \\markuplines \\table-of-contents\n"))
(if (d-Directive-paper? tag)
	(set! title (d-DirectiveGet-paper-display tag)))
(set! title (d-GetUserInput "Table of Contents Title" "Give title for the table of contents\nBlank to delete" title))
(set! pb (d-GetUserInput "Table of Contents Page Break Control" "Give 0 - No page breaks\n1 - Page Break before Table of Contents\n2 - Page Break after\n3 - Both" pb))

(if pb
	(cond ((equal? pb "1")
			(set! mc (string-append "\n\\pageBreak\n" mc)))
		    ((equal? pb "2") 
		    	(set! mc (string-append mc "\n\\pageBreak\n")))
		    ((equal? pb "3") 
		    	(set! mc (string-append  "\n\\pageBreak\n" mc "\\pageBreak\n")))))

(if (string-null? title)
	(begin
		(d-DirectiveDelete-paper tag)
		(d-DirectiveDelete-movementcontrol tag))
	(begin
		(d-DirectivePut-paper-postfix tag (string-append "\ntocTitle = \"" title "\"\n"))
		(d-DirectivePut-paper-display tag title)
		(d-DirectivePut-movementcontrol-prefix tag mc)))
(d-SetSaved #f))
(d-PopPosition)