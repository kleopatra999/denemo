;;MouseInsertion
(d-AddKeybinding "InsertOneNote" "PrsL-Shift")
(d-AddKeybinding "AddNoteToChord" "PrsL-Alt")
(d-AddKeybinding "StagedDelete" "PrsL-Control")
(d-AddKeybinding "A" "a")
(d-AddKeybinding "B" "b")
(d-AddKeybinding "C" "c")
(d-AddKeybinding "D" "d")
(d-AddKeybinding "E" "e")
(d-AddKeybinding "F" "f")
(d-AddKeybinding "G" "g")
(d-AddKeybinding "Set0" "Shft+0")
(d-AddKeybinding "Set1" "Shft+1")
(d-AddKeybinding "Set2" "Shft+2")
(d-AddKeybinding "Set3" "Shft+3")
(d-AddKeybinding "Set4" "Shft+4")
(d-AddKeybinding "Set5" "Shft+5")
(d-AddKeybinding "Set6" "Shft+6")

(CreateButton "Note0" "𝅝")
(d-SetDirectiveTagActionScript "Note0" "(d-Set0)")
(CreateButton "Note1" "𝅗𝅥")
(d-SetDirectiveTagActionScript "Note1" "(d-Set1)")
(CreateButton "Note2" "𝅘𝅥")
(d-SetDirectiveTagActionScript "Note2" "(d-Set2)")
(CreateButton "Note3" "𝅘𝅥𝅮")
(d-SetDirectiveTagActionScript "Note3" "(d-Set3)")
(CreateButton "Note4" "𝅘𝅥𝅯")
(d-SetDirectiveTagActionScript "Note4" "(d-Set4)")
(CreateButton "Note5" "𝅘𝅥𝅰")
(d-SetDirectiveTagActionScript "Note5" "(d-Set5)")
(CreateButton "Note6" "𝅘𝅥𝅱")
(d-SetDirectiveTagActionScript "Note6" "(d-Set6)")

(CreateButton "SharpenNext" "♯")
(d-SetDirectiveTagActionScript "SharpenNext" "(d-PendingSharpen)")
;MouseInsertion
(CreateButton "FlattenNext" "♭")
(d-SetDirectiveTagActionScript "FlattenNext" "(d-PendingFlatten)")

(CreateButton "Dot - Double Dot - No Dot" "•")
(d-SetDirectiveTagActionScript "Dot - Double Dot - No Dot" "(d-DotDoubleDotNoDot)")

(CreateButton "BeginSlur" "(")
(d-SetDirectiveTagActionScript "BeginSlur" "(d-ToggleBeginSlur)")
(CreateButton "EndSlur" ")")
(d-SetDirectiveTagActionScript "EndSlur" "(d-ToggleEndSlur)")

(CreateButton "Tie" "☊")
(d-SetDirectiveTagActionScript "Tie" "(d-ToggleTie)")

(CreateButton "Triplets" "3")
(d-SetDirectiveTagActionScript "Triplets" "(d-ToggleTripleting)")

(CreateButton "Dynamics" "<span font=\"times\" size=\"large\" font_style=\"italic\" font_weight=\"bold\">f p</span>")
(d-SetDirectiveTagActionScript "Dynamics" "(d-DynamicsDialog)")

(CreateButton "Tempo" "<span font=\"times\" size=\"large\" font_style=\"italic\" font_weight=\"bold\">Allegro</span>")
(d-SetDirectiveTagActionScript "Tempo" "(d-MetronomeMark)")

(CreateButton "StartCresc" "<span font=\"times\" size=\"large\" font_weight=\"bold\">&lt;</span>")
(d-SetDirectiveTagActionScript "StartCresc" "(d-ToggleStartCrescendo)")
(CreateButton "EndCresc" "<span font=\"times\" size=\"large\" font_weight=\"bold\">&lt;|</span>")
(d-SetDirectiveTagActionScript "EndCresc" "(d-ToggleEndCrescendo)")

(CreateButton "StartDim" "<span font=\"times\" size=\"large\" font_weight=\"bold\">&gt;</span>")
(d-SetDirectiveTagActionScript "StartDim" "(d-ToggleStartDiminuendo)")
(CreateButton "EndDim" "<span font=\"times\" size=\"large\" font_weight=\"bold\">&gt;|</span>")
(d-SetDirectiveTagActionScript "EndDim" "(d-ToggleEndDiminuendo)")

(if DenemoPref_newbie
	(d-InfoDialog (_ "To insert notes hold down shift key while clicking the mouse. To add to a chord hold down Alt key, to delete hold down Control key while clicking.")))
