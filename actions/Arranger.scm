(if (and (= 1 (d-GetMovement)) (not (d-Directive-header? "ScoreTitle")))
(SetHeaderField "title" "Untitled"))
(d-DirectiveDelete-scoreheader "ScoreTagline")
(d-LimitInterSystemSpace 1.2)
