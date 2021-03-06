;;;MultiLineTextAnnotation
(let ((text #f) (params  MultiLineTextAnnotation::params) (tag "MultiLineTextAnnotation")(markup #f)(current #f)(scale "0.5")
        (x-offset #f)
        (y-offset #f)
        (prefix "<>")
        (dimensions "")
        (dim #f)
        (data #f))

     (set! data (d-DirectiveGet-standalone-data tag))
     (if data
        (set! data (eval-string data))
        (set! data '()))
    (set! dim (assq-ref data 'dimensions))    
    (set! current (assq-ref data 'text))
    (set! scale (assq-ref data 'scale))
    (set! x-offset (assq-ref data 'x-offset))
    (set! y-offset (assq-ref data 'y-offset))
    
    (if (list? params)
    	(begin
    		(if  (assq-ref params 'dimensions)
    			(set! dim  (assq-ref params 'dimensions)))
   		(if  (assq-ref params 'text)
    			(set! current  (assq-ref params 'text)))
   		(if  (assq-ref params 'scale)
    			(set! scale  (assq-ref params 'scale)))      			   
   		(if  (assq-ref params 'x-offset)
    			(set! x-offset  (assq-ref params 'x-offset)))      			   
   		(if  (assq-ref params 'y-offset)
    			(set! y-offset  (assq-ref params 'y-offset)))
    			(set! params 'finished)))  			   

    (if (not scale)
        (set! scale "1"))
    (if (and  (d-Directive-standalone? tag) current)
        (set! prefix (d-DirectiveGet-standalone-prefix tag)))
    (if (equal? "edit" params)
        (let ((choice (RadioBoxMenu (cons (_ "Edit Text") #f) 
        (cons (_ "Edit Space Occupied") 'space)
        (cons (_ "Edit Size") 'scale)
         (cons (_ "Edit Position") 'position))))
            (case choice
              ((scale) 
              	(set! params #f))
                ((space) 
                    (begin
                    	(set! params #f)
                        (set! dim (d-GetUserInput (_ "Space Occupied by Text/Music") (_ "Give space:\n(0 prevents dragging position\nBlank for natural size.)") dim)))
                        (if (not (and dim (string->number dim) (>= (string->number dim) 0)))
                                    (set! dim #f)))
                ((position) 
                    (set! x-offset (d-GetUserInput (_ "Offsets")  (_ "Give horizontal displacement required") x-offset))
                    (set! y-offset (d-GetUserInput (_ "Offsets")  (_ "Give vertical displacement required") y-offset))      
                    (if (not (and x-offset (string->number x-offset) y-offset (string->number y-offset)))
                        (begin
                            (set! x-offset #f)
                            (set! y-offset #f)))
                    (set! params #f)))))

    (if (and y-offset x-offset)
        (set! prefix (string-append "<>-\\tweak #'extra-offset #'(" x-offset " . " y-offset ") ")))        
        
         

    (if dim
        (set! dimensions (string-append "\\with-dimensions #'(-" dim " . " dim ") #'(-" dim " . " dim ")")))
            
    (if (not current)
                (set! current ""))
                
    (if (not dimensions)
                (set! dimensions ""))
    (if (not text)
        (set! text (d-GetUserInputWithSnippets (_ "Text") (_ "Give text to appear with following note/chord:\nThe characters \\, \", §, { and } have a special meaning in the text,\nthe backslash \\ starts some LilyPond sytax, the others must be paired.\nTo apply italic or bold to a group of words enclose them in {}, e.g. \\bold {These words are bold}.\nOther markup commands \\super, \\tiny etc, see LilyPond documentation.") current)))
    (if text 
       (begin
       	   (if (not params)
           	 (set! scale (d-GetUserInput (_ "Scaling Text") (_ "Give text size: ") scale)))
            (if (not scale) 
                (set! scale "0.5"))
            (set! markup (cdr text))
            (set! text (car text))
            (set! data (assq-set! data 'text text))
            (set! data (assq-set! data 'scale scale))
            (if dim 
                (set! data (assq-set! data 'dimensions dim))
                (set! data (assq-remove! data 'dimensions)))
 
            (if x-offset
             (set! data (assq-set! data 'x-offset x-offset)))
            (if y-offset
             (set! data (assq-set! data 'y-offset y-offset)))
                       
            (if (not (d-Directive-standalone? tag))
                (d-DirectivePut-standalone tag))
            (d-DirectivePut-standalone-data tag (format #f "'~s" data))
            (d-DirectivePut-standalone-display tag text)
            (d-DirectivePut-standalone-postfix tag (string-append "^\\markup"dimensions"\\scale #'(" scale " . " scale ")\\column{" markup "}"))
            (d-DirectivePut-standalone-prefix tag prefix)
            (d-DirectivePut-standalone-minpixels tag 30)
            (d-RefreshDisplay)
            (d-SetSaved #f))
        (begin
            (if (not params)
                (let ((confirm (d-GetUserInput (d-DirectiveGet-standalone-display tag) (_ "Delete this text?") (_ "y"))))
                 (if (and confirm (equal? confirm (_ "y")))
                    (begin
                        (d-DirectiveDelete-standalone tag)
                        (d-SetSaved #f))
                    (d-InfoDialog (_ "Cancelled"))))))))