; -*- text -*-
;
; VBI and Display List Interrupts
;

 .local

@init_vbi
     lda VVBLKI
     sta ?store_vvblki
     lda VVBLKI+1
     sta ?store_vvblki+1

     jsr @stop_displaylist_interrupts

     LDA #6           ; use inferred(6) VBI
     LDY #<?GAME_VBI
     LDX #>?GAME_VBI
     JMP SETVBV

@stop_displaylist_interrupts
    lda #$40
    sta ?nmien_value
    rts


@start_displaylist_interrupts
    lda #$c0
    sta ?nmien_value
    rts

?nmien_value
 .byte 0

?store_vvblki
 .word 0

@deinit_vbi
     LDA #6         ; use inferred(6) VBI
     LDY ?store_vvblki
     LDX ?store_vvblki+1
     JMP SETVBV

;
; OO   OO OOOOO  OOOO
; OO   OO OO  OO  OO
; OO   OO OO  OO  OO
; OO   OO OOOOO   OO
; OO   OO OO  OO  OO
;  OO OO  OO  OO  OO
;   OOO   OOOOO  OOOO
;


?GAME_VBI
     LDA #0
     STA 77              ; no colorswitch

     LDA #$0
     STA NMIEN           ; Displaylist Interrupt must not occur

     jsr @reset_meter

; Show Game Screen
     LDA #<@displaylist  ; Game Screen
     STA SDLSTL
     LDA #>@displaylist
     STA SDLSTL+1

     LDA #<?DLI
     STA VDSLST
     LDA #>?DLI
     STA VDSLST+1

     LDA ?nmien_value
     STA NMIEN           ; Displaylist Interrupt are possible
     JMP SYSVBV


; OOOOO  OO     OOOO
; OO  OO OO      OO
; OO  OO OO      OO
; OO  OO OO      OO
; OO  OO OO      OO
; OO  OO OO      OO
; OOOOO  OOOOOO OOOO

?DLI
 pha

 lda #8               ; shows a gray line
 sta colbk

 jsr @multi_sprite_dli_sub

 lda #0
 sta colbk

 pla
 rti
