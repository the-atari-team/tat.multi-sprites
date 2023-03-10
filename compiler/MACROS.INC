; -*- text -*-
;Macros
;fuer den WiNiFe-Compiler
;cdw by 'The Atari Team' 1989-2022


; Increment a word value by one
    .MACRO WINC
      INC %1   ; 5
      BNE W    ; 2+1
      INC %1+1 ; 5
W
    .ENDM

    .macro move.b
    lda %1
    sta %2
    .endm

    .macro move.w
    move.b %1,%2
    move.b %1+1,%2+1
    .endm

; decrement a word value by one
    .MACRO WDEC
      LDA %1+0       ;Test if the LSB is zero
      BNE SKIP       ;If it isn't we can skip the next instruction
      DEC %1+1       ;Decrement the MSB when the LSB will underflow
SKIP  DEC %1+0       ;Decrement the LSB
    .ENDM

; @deprecated
; wir coden das jetzt selbst
; Folgendes Macro wird verwendet um den Heap_Ptr zu erhoehen,
; wenn Proceduren mit Parametern aufgerufen werden.
; 11 Bytes (statt 6 Bytes (2+3+1) (lda #, jsr x, rts))
; aber 13 Takte statt 24 mit jsr
    .MACRO ADD_TO_HEAP_PTR
      CLC              ;2 (Takte)
      LDA @HEAP_PTR    ;3
      ADC #%1          ;2
      STA @HEAP_PTR    ;3
      BCC NO_INC_HIGH_HEAP_PTR ;3 ; => 13 Takte statt 24 mit jsr

      INC @HEAP_PTR+1  ;5

NO_INC_HIGH_HEAP_PTR
    .ENDM


; @deprecated
; wir coden das jetzt selbst
; Folgendes Macro wird verwendet um den Heap_Ptr zu erniedrigen,
; wenn Proceduren mit Parametern aufgerufen wurden.
; 11 Bytes (statt 6 Bytes (2+3+1) (lda #1, jsr x, rts))
; aber 13 Takte statt 32 mit jsr
    .MACRO SUB_FROM_HEAP_PTR
      SEC              ;2
      LDA @HEAP_PTR    ;3
      SBC #%1          ;2
      STA @HEAP_PTR    ;3
      BCS NO_DEC_HIGH_HEAP_PTR ;3 ; => 13 Takte statt 30 mit jsr

      DEC @HEAP_PTR+1  ;5

NO_DEC_HIGH_HEAP_PTR
    .ENDM

; To store a word value into a word array, we use @PUTARRAY Pointer
; to support more optimizations, we use (A + 256 * X) here
; first we need to multiply pointer by 2, then add variable
; Putarrayw Var
    .MACRO PUTARRAYW
;      TYA         ; 0 putarrayw MACRO
      ASL A       ; 2 Mult (x,y)*2
      TAY         ; 2
      TXA         ; 2
      ROL A       ; 2
      TAX         ; 2
      CLC         ; 2 add %1 to the nth value
      TYA         ; 2
      ADC # <%1   ; 2
      STA @PUTARRAY ; 3
      TXA         ; 2
      ADC # >%1   ; 2
      STA @PUTARRAY+1 ; 3
    .ENDM
;         in Summe 26 Zyklen 16 Bytes


; To get a word value from a word array, we use @GETARRAY pointer
; to support some more optimizations we use (A + 256 * X) for set pointer
; first we need to multiply pointer by 2, then add variable
; and get the result in akku, extern we need to move it to Y-Register
; Getarrayw Var
    .MACRO GETARRAYW
;      TYA           ; 0 Getarrayw MACRO
      ASL A         ; 2 Mult (x,y)*2
      TAY           ; 2
      TXA           ; 2
      ROL A         ; 2
      TAX           ; 2
      CLC           ; 2 add %1 to the nth value
      TYA           ; 2
      ADC # <%1     ; 2
      STA @GETARRAY ; 3
      TXA           ; 2
      ADC # >%1     ; 2
      STA @GETARRAY+1 ; 3
      LDY #1        ; 2 load x and y from @ARRAY
      LDA (@GETARRAY),Y ; 5
      TAX           ; 2
      DEY           ; 2
      LDA (@GETARRAY),Y ; 5
;      TAY           ; 0
    .ENDM
;          in Summe 42 Zyklen 23 Bytes
