; -*- text -*-
;BASIC-Library
;fuer den WiNiFe-Compiler
;cdw by 'The Atari Team' 1989-2021
;
; TODO: Would be nice that the default is to stop our programm.
;

    .LOCAL

; Small helper, to allow break-key in our Assembler routines
; will stop via interrupt.

; TODO: set last PC in BININT as return value

; set breakkey interrupt handler to our own routine
; This function is called, if we add
; include 'BREAKKEY.INC' to our includes
@INIT_BREAKKEY
    lda BRKKY
    sta ?old_breakkey_interrupt
    lda BRKKY+1
    sta ?old_breakkey_interrupt+1 ; save current handler

    lda #<?break_key_interrupt    ; set our own break key handler
    sta BRKKY
    lda #>?break_key_interrupt
    sta BRKKY+1

    LDA POKMSK                    ; allow break key
    ORA #$80
    STA POKMSK

    lda #1
    sta ?break_initialised
    RTS

?break_initialised
    .byte 0

?old_breakkey_interrupt
    .word 0

; restore old break key pointer
; is called, if include 'BREAKKEY.INC' is added
@RESTORE_BREAKKEY
; restore breakkey interrupt handler

    lda ?break_initialised
    beq ?break_not_initialised

    lda ?old_breakkey_interrupt
    sta BRKKY
    lda ?old_breakkey_interrupt+1
    sta BRKKY+1                       ; restore old saved handler

?break_not_initialised
    lda #0
    sta ?break_initialised

    RTS

; this is our own break key interrupt handler, will called when break key is pressed
?break_key_interrupt
    cld
    cli        ; Interrupts zulassen
    lda #0     ;
    sta BRKKEY ; :=0, breakkey wurde gedrueckt

    jsr @RESTORE_BREAKKEY
    jmp @exit  ; exit rufen, dabei wird per tsx der Stackpointer zurueckgesetzt
