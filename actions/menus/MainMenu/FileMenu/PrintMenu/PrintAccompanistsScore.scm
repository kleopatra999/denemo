;;;PrintAccompanistsScore
(let ((saved (d-GetSaved)) (response (d-GetUserInput "Print Accompanist's Score" "How many staffs to make small?" "1")))
  (if response
      (let ((staffs ""))
    (set! response (string->number response))
    (let loop ((number 1))
      (set! staffs (string-append staffs "(d-GoToPosition #f " (number->string number) " 1 1) (d-SmallerStaff)"))
      (if (< number response)
          (loop (+ 1 number)))) 
          
    (let ((name (string-append "Accompanist with " (number->string response) " cue part" )))      
         (d-DeleteLayout name)  
        (ForAllMovements staffs)
        (d-TypesetForScript "(d-PrintAccompanistsScore)")
        (d-CreateLayout name))
    (ForAllMovements staffs)
    (d-SetSaved saved))))
