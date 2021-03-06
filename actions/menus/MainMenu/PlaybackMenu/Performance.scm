; d-Performance
(d-CreateTimebase)
(d-MoveToBeginning)
(define Performance::timings '())
(define (Performance::play)
 (if (not (null? Performance::timings))
  (begin
    (d-SetPlaybackInterval (car (car (car Performance::timings))) (cdr (car Performance::timings)))
     (apply d-GoToPosition (cdr (car (car Performance::timings))))
    (set! Performance::timings (list-tail Performance::timings 1))
    (d-Play "(Performance::play)"))))
    
(let ((beginning (GetPosition))(position #f)(s1 #f)(e1 #f)(s2 #f)(e2 #f)(fine #f)(segno #f)(segno-position #f)(timing 0.0))
  (set! s1 (cons 0.0 beginning))
  (while (d-NextObject)
    (if (d-GetMidiOffTime)
      (set! timing (d-GetMidiOffTime)))
    ;(disp "MIDI off time " timing " ok")
    (set! position (GetPosition))
    (cond
      ((Music?)
          (if (d-Directive-chord? "DCAlFine")
            (begin
              ;(disp "DC al fine performing " s1 " to " timing "first")
              (set! Performance::timings (cons (cons s1 timing) Performance::timings))
              (set! s1 (cons 0.0 beginning))   
              (set! e1 timing)
              (if fine
                (set! e1 fine)
                (d-InfoDialog (_ "No fine found, assuming all")))))
           (if (d-Directive-chord? "DalSegno")
            (begin
              (set! Performance::timings (cons (cons s1 timing) Performance::timings))
              (set! s1 (cons 0.0 beginning)) ;; in case no segno found
              (if segno
                (set! s1 (cons segno segno-position))
                (d-InfoDialog (_ "Dal Segno with no Segno - assuming Da Capo")))
              (set! e1 timing)
              (if fine
                (set! e1 fine)
                (d-InfoDialog (_ "No fine found, assuming all")))))
          (if (d-Directive-chord? "ToggleFine")
              (set! fine timing))
           (if (d-Directive-chord? "ToggleSegno")
              (begin
                (set! segno-position (GetPosition))
                (set! segno (d-GetMidiOnTime))))
          )
      ((or (d-Directive-standalone? "RepeatEnd") (d-Directive-standalone? "RepeatEndStart"))
          (set! e1 timing)
          ;(disp "Midi repeat from " e1 " ok")
          (set! Performance::timings (cons (cons s1 e1) Performance::timings))
          ;(disp "timings at repeat end " Performance::timings " ok")
          (if (not e2)
                (set! e2 e1))
          (if (not s2)
                (set! s2 s1))
            
          (set! Performance::timings (cons (cons s2 e2) Performance::timings))
          ;(disp "timings at repeat " Performance::timings " ok")
          (set! s1 (cons timing (GetPosition)))
          (set! e1 #f)
          (set! s2 #f)
          (set! e2 #f)
          (if (d-Directive-standalone? "RepeatStart")
            (set! s2 (cons timing (GetPosition))))
          )
      ((d-Directive-standalone? "RepeatStart")
          (set! s2 (cons timing (GetPosition))))
      ((d-Directive-standalone? "Volta1")
        (set! e2 timing)
        (disp "Midi 2 repeat to " e2 " ok")
        (if (not s2)
          (set! s2 (cons 0.0 beginning)))
          )
    )
  )
(if (not e1)
  (set! e1 timing))
(if (not (equal? (car s1) timing))
    (set! Performance::timings (cons (cons s1 e1) Performance::timings)))
(set! Performance::timings (reverse Performance::timings))
(Performance::play))
