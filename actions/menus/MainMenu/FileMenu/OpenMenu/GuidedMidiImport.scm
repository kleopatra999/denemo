;;;GuidedMidiImport
(if  (d-New)
(begin
	(d-ImportMidi "guided=true")
	(d-CreateClickStaffForMidi)
	(d-LoadMidiTrack)
	(d-InfoDialog "You can now insert the notes from the MIDI track into the staff beneath the (top) click track staff.
	The MIDI notes detected are displayed above the top staff, and the first is marked ready to be entered.
	The Return key will enter the note using the duration calculated from the time until the next note -
	override this by playing a duration key 0, 1 .... or clicking a duration button.
	Use the Ins key to insert a note in the chord if the MIDI notes are not sequential.
	You can use the Navigation commands to move the marked MIDI note around if you need to back up or re-start.
	When you have finished with this MIDI track use the button to load another track. 
	It is best to work up from the lowest numbered track so that the staffs are in order without having to swap them afterwards.")
	(d-ShowPalettes "Midi Import")
	(d-AddKeybinding "InsertMarkedMidi" "Return")))
