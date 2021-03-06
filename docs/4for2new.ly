\version "2.0.1"
% Mudela file generated by Denemo version 0.7.2a

%http://www.gnu.org/software/denemo/denemo.html

blanknotes = {
\property Voice.NoteHead \override
	#'transparent = ##t
\property Voice.Stem \override
	#'transparent = ##t 
 }
unblanknotes = {\property Voice.NoteHead \revert #'transparent
\property Voice.Stem \revert #'transparent 
}
\header{
	title = "Four Short Piece for Two Oboes"
	subtitle = ""
	composer = "Aaron Mehl"
	opus = "3-20-1979"
}

\include "paper16.ly"
OboeI = \notes \context Voice = OboeI {
	\property Staff.midiInstrument = "oboe"
	\time 4/4
	\key d \minor
	\clef treble
	\tempo 4=120 


d''16\mf( e'' f'' g'') \acciaccatura{a''8(}  bes''8)( a''16 g'') c'''8( a''16 g'' f''4) |
	\time 4/4 g''\mordent->( f''8 e'' d''4)\dim a'-( |
	\times 2/3 { g'8 f' e' }\enddim  d'2.-) \!   |
	r16  c''( d'' c'') \times 2/3 { bes'8 (a' g' } f'2) |
%5
	d''16( e'' f'' g'') \acciaccatura{a''8(} bes''8)( a''16 g'') c'''8( a''16 g'' f''4) |
	g''\mordent-> ( f''8 e'' d''4) a' |
	\times 2/3 { g'8 f' e' } d''2 a'4 |
	bes'8 a' f' g' a'2) |
	\time 5/4 d''4( a' \times 2/3 { g'8 f' e' } d'2) |
%10
	bes'4( a'8 g') c''4( a'8 g' f'4 |
	a'8 bes' a'4 f'8 g' a'2) |
	d''8 c''16 bes' a'4 \times 2/3 { g'8 f' e' } d'2 |
	bes'4 a'8 g' c''4 a'8 g' f'4 |
	\time 3/4 r a'2 |
%15
	\time 4/4 r4 a'16 g' c''8 r4 g'16 f' e'8 |
	\time 7/4 a'16 bes' a' f' g'8 a' r2. a''4 bes'' |
	\time 3/4 c'''16 d''' c''' bes'' a''4 r |
	\time 5/8 f'' d''16 e'' f'' g'' a''8 |
	\time 6/8 a''4 r8 a'16 g' f'4 |
%20
	\time 6/4 a'16 bes' a' f' g'8 a' f'8. e'16 d' c' d' e' f'8 g' a'4 |
	\time 3/4 bes' c''16 d'' c'' bes' a'4 |
	\time 4/4 bes' c'' c'' r |
	d''16 e'' f'' g'' bes''8 a''16 g'' c'''8 a''16 g'' f''4 |
	g'' f''8 e'' d''4 a' |
%25
	\times 2/3 { g'8 f' e' } d'2. |
	r16 c'' d'' c'' \times 2/3 { bes'8 a' g' } f'2 |
	d''16 e'' f'' g'' bes''8 a''16 g'' c'''8 a''16 g'' f''4 |
	g'' f''8 e'' d''4 a' |
	\times 2/3 { g'16 f' e' } d''8 a'4 r r |
%30
	\time 3/4 r e'16 d' c' d' d''8 a'' |
	\times 2/3 { g''16 f'' e'' } d''8 \times 2/3 { a'16 g' f' } e'8 \times 2/3 { d'16 cis' d' } a'8 |
	\time 6/4 r4 a'8 d'' d'16 c' f'8 e'16 d' g'8 f'16 e' a'8 g'16 f' bes'8 |
	\time 3/4 d'' c''16 bes' a'8 \times 2/3 { g'16 f' e' } d'4 |
	f''8 e''16 d'' c''8 \times 2/3 { bes'16 a' g' } f'4 |
%35
	\time 4/4 \times 2/6 { d'16 e' f' g' a' bes' } c'' d'' c''' a'' g'' a'' bes'' c''' d'''4 |
	a''2 r \bar "|."
}
OboeIi = \notes \context Voice = OboeIi {
	\property Staff.midiInstrument = "oboe"
	\time 4/4
	\key d \minor
	\clef treble
	r4\mf d'16 e' f' g' a'8 bes' a'16 g' c''8 |
	\time 4/4 a'16 g' f'8 g'4 r f'16 g' f' e' |
	d'4 f'4. e'8 d' cis' |
	d'16 e' f' g' a'2. |
%5
	r4 d'16 e' f' g' a'8 bes' a'16 g' c'' a' |
	g' f' g' e' g'4 f'16 g' f' e' d' e' f' g' |
	d'4 f'4. e'8 d' cis' |
	d'4 bes'8 bes'' a''2 |
	\time 5/4 r4 d'' a' \times 2/3 { g'8 f' e' } d'4 ~ |
%10
	d' bes' a'8 g' c''4 a'8 g' |
	f' g' f' e' c'4 d'2 |
	r4 d'' a' \times 2/3 { g'8 f' e' } d'4 ~ |
	d' bes' a'8 g' c''4 a'8 g' |
	\time 3/4 f' g' a'2 |
%15
	\time 4/4 r a'16 g' f'8 r4 |
	\time 7/4 r2 f'8. e'16 d' c' d' e' f'8 g' r4 a' |
	\time 3/4 r2 g''4 |
	\time 5/8 r r r8 |
	\time 6/8 r4 a''16 g'' c'''8 g'' f'' |
%20
	\time 6/4 e''4 d'' a' \times 2/3 { g'8 f' e' } d'2 |
	\time 3/4 g'4 a' r |
	\time 4/4 g'16 f' e' d' f' g' a' c'' g' f' e' d' r4 |
	a' d'16 e' f' g' a'8 bes' a'16 g' c''8 |
	a'16 g' f'8 g'4 g' f'16 g' f' e' |
%25
	d'4 f'4. e'8 d' cis' |
	d'16 e' f' g' a'2. |
	r4 d'16 e' f' g' a'8 bes' a'16 g' c'' a' |
	g' f' g' e' g'4 f'16 g' f' e' d' e' f' g' |
	d'8 f' e'16 d' cis' d' r4 r |
%30
	\time 3/4 e'16 d' cis' d' d''8 a' \times 2/3 { g'16 f' e' } d'8 |
	r \times 2/3 { e''16 d'' c'' } d''8 \times 2/3 { a'16 g' f' } e'8 d' |
	\time 6/4 \times 2/3 { e'16 f' g' } a'8 d'' d'16 cis' r4 r d'16 cis' f'8 e'16 d' a'8 |
	\time 3/4 r d'' c''16 bes' a'8 g' a' |
	r f'' e''16 d'' c''8 \times 2/3 { bes'16 a' g' } g'8 |
%35
	\time 4/4 \times 2/6 { a'16 bes' a' f' g' a' } \times 2/6 { a' bes' a' f' g' a' } \times 2/6 { a' bes' a' f' g' a' } \times 2/5 { a' bes' a' f' g' } |
	a'2 r \bar "|."
}

OboeIStaff = \context Staff = OboeIStaff <<
	\OboeI
>>


OboeIiStaff = \context Staff = OboeIiStaff <<
	\OboeIi
>>

\score {
	<<
		\OboeIStaff
		\OboeIiStaff
	>>
	\paper {
	}
	\midi {
		\tempo 4 = 120
	}
}
