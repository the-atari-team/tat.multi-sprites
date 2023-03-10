; -*- text -*-
; Multisprite Multiplexer
;cdw by 'The Atari Team' 2022
;
; Written for Night Knight
;
; based on Shanti77 & tebe code, thanks for that code!

; The Atari 8bit has 4 Players. they take the whole screen in the hight.
; This code helps to display more than 16 sprites on a screen at the same time. 8bit wide, 16 lines high.
; We need a DLI every 8th display line, they must be at the right position.
; If we accept flickering, more than 4 sprites can be shown in the same horizontal height.
; But you should limit it to 8 sprites the same horizontal height.
; On old tube monitor with a slightly trailing plasma layer these flickering is not very strong.
; In Altirra Emulator you can set Frame blending on in the video configuration.
; On current LED/LCD/OLED Displays it will flicker little bit stronger.
;
; You must create a
; word array @all_sprite_data_table[1] = [...]
; Every sprite data to display need an entry in this table.
; The value of @sprite_shape[i] shows into this table and copy it's data to the player memory to show on screen.
; Which player is used for display isn't fix and vary, only the first sprite is fix and will also NOT flicker.
; Use the first sprite for your own game figure.
; Collision detection is a challenge.
;
; The source code is a little bit lengthy, we use lot of unroll-loop here.
; Especially when writing the data, no loop constructs are used.
;
; Later tests will have to show how many sprites can be displayed on NTSC systems
;
; Restrictions:
; You can't draw sprites over the first DLI + 8 screen lines
; You can't draw sprites under the last DLI
; You should not show more than 8 sprites in horizontal

 .local

pmadr=$B000

 .align 16

@sprite_x
 .DC MAX_SPRITES 0

@sprite_y
 .DC MAX_SPRITES 0

@sprite_shape
 .DC MAX_SPRITES 0

@sprite_color
 .DC MAX_SPRITES 0

; Sprite initialization
@init_sprites
    lda #MAX_SPRITES-1
    sta last_sprite

    lda #0
    tax
?loop2
    sta pmadr+$300,x        ; clear Player/Missiles memory
    sta pmadr+$400,x
    sta pmadr+$500,x
    sta pmadr+$600,x
    sta pmadr+$700,x
    dex
    bne ?loop2

    rts

;
;     OO                                                    OO    OO
;     OO                                                          OO
;  OOOOO OOOOO   OOOOO OO   OO         OOOO  OOOOO  OOOOO  OOO  OOOOOO  OOOO   OOOO
; OO  OO OO  OO OO  OO OO   OO OOOOOO OO     OO  OO OO  OO  OO    OO   OO  OO OO
; OO  OO OO     OO  OO OO O OO         OOOO  OO  OO OO      OO    OO   OOOOOO  OOOO
; OO  OO OO     OO  OO OO O OO            OO OOOOO  OO      OO    OO   OO         OO
;  OOOOO OO      OOOOO  OO OO          OOOO  OO     OO     OOOO    OOO  OOOO   OOOO
;                                            OO

; draws always all sprites
; may only start from VCOUNT >= 105, otherwise it flickers heavily and display errors occur
@show_sprites
    ldy #0
    sty spr_flag

    lda @sprite_x           ; only sprites with X-Position != 0 will display
    beq ?no_spieler

    jsr ?print_sprite        ; player sprite 0 never flicker!
                             ; TIP: Take the lightest color! The darker, the less flickering is noticeable.
?no_spieler
    ldy last_sprite

?loop3
    lda @sprite_x,y
    beq ?no_print_sprite

    jsr ?print_sprite        ; Y register contains sprite number, Akku the X position

?no_print_sprite
    dey
    bne ?there_are_more_sprites
    ldy #MAX_SPRITES-1

?there_are_more_sprites
    cpy last_sprite
    bne ?loop3


    lda spr_flag
    beq ?no_start
    sta last_sprite

?no_start
    rts




; Draw a sprite. current number [0 to MAX_SPRITE-1] in Y register
?print_sprite
;    lda #$32
;    sta colbk

    lda @sprite_y,y    ; bereits gelesen

;    cmp #POSY_MIN     ; check Y position, code is not need here
;    bcc quit
;    cmp #POSY_MAX
;    bcs quit

; We have block lines (each 8 real lines high) in here a sprite is moving
    sta block_y
    lsr
    lsr
    lsr
    tax                ; first block number

    lda block_y
    and #~00000111
    sta block_dy       ; line within a block

    bne ?no_dex
    dex                ; if dy == 0, then decrement the number of the first block
?no_dex

    lda block_status,x
    ora block_status+1,x
    ora block_status+2,x
    lsr
    bcc ?print_sprite_0
    lsr
    bcc ?print_sprite_1
    lsr
    bcc ?print_sprite_2
    lsr
    bcc ?print_sprite_3

; here it is not possible to draw a sprite
    lda spr_flag
    bne ?quit
    sty spr_flag
?quit
    rts

; Y register contains sprite number [0 bis MAX_SPRITES-1]
; X register contains current Y position on the screen (*8 for real height)
?print_sprite_0
    inc block_status,x
    inc block_status+2,x

; Set the position and color of sprites in the block
    lda block_x0+3,x
    bne ?no_inc
    inc block_x0+3,x
?no_inc
    lda @sprite_x,y        ; sprite 0
    sta block_x0,x

    lda @sprite_color,y
    sta block_c0,x

    ldx @sprite_shape,y    ; in X register is now the sprite shape content
    jmp ?shape_0

?print_sprite_1
    lda #2
    ora block_status,x    ; Claim the selected sprite in status
    sta block_status,x
    lda #2
    ora block_status+2,x
    sta block_status+2,x


    lda block_x1+3,x
    bne ?no_inc2
    inc block_x1+3,x
?no_inc2
    lda @sprite_x,y        ; sprite 1
    sta block_x1,x

    lda @sprite_color,y
    sta block_c1,x

    ldx @sprite_shape,y
    jmp ?shape_1

?print_sprite_2
    lda #4
    ora block_status,x    ;Claim the selected sprite in status
    sta block_status,x
    lda #4
    ora block_status+2,x
    sta block_status+2,x


    lda block_x2+3,x
    bne ?no_inc3
    inc block_x2+3,x
?no_inc3
    lda @sprite_x,y        ; sprite 2
    sta block_x2,x

    lda @sprite_color,y
    sta block_c2,x

    ldx @sprite_shape,y
    jmp ?shape_2

?print_sprite_3
    lda #8
    ora block_status,x    ;Claim the selected sprite in status
    sta block_status,x
    lda #8
    ora block_status+2,x
    sta block_status+2,x


    lda block_x3+3,x
    bne ?no_inc4
    inc block_x3+3,x
?no_inc4
    lda @sprite_x,y        ; sprite 3
    sta block_x3,x

    lda @sprite_color,y
    sta block_c3,x

    ldx @sprite_shape,y
    jmp ?shape_3



?shape_0
    lda @all_sprite_data_table_low,x
    sta data
    lda @all_sprite_data_table_high,x
    sta data+1

    sty nr_sprite
    ldx block_y                   ; Y position in screen

?set_sprite_0
    jsr player0a                  ; draw the sprite

    ldy block_dy
    lda tab_clear_sprite_0_low,y
    sta ?clear_sprite_0+1
    lda tab_clear_sprite_0_high,y
    sta ?clear_sprite_0+2
    ldy nr_sprite

;    lda #$81 ; with this byte it is very easy to see how the players are used.
    ldx block_y                   ; Y position in screen
    lda #0
?clear_sprite_0
    jmp cls0                     ; jump to the clear routine

?shape_1
    lda @all_sprite_data_table_low,x
    sta data
    lda @all_sprite_data_table_high,x
    sta data+1

    sty nr_sprite
    ldx block_y

?set_sprite_1
    jsr player1a

    ldy block_dy
    lda tab_clear_sprite_1_low,y
    sta ?clear_sprite_1+1
    lda tab_clear_sprite_1_high,y
    sta ?clear_sprite_1+2

    ldy nr_sprite
    ldx block_y                   ; Y position in screen
    lda #0

?clear_sprite_1
    jmp cls0b


?shape_2
    lda @all_sprite_data_table_low,x
    sta data
    lda @all_sprite_data_table_high,x
    sta data+1

    sty nr_sprite
    ldx block_y
?set_sprite_2
    jsr player2a

    ldy block_dy
    lda tab_clear_sprite_2_low,y
    sta ?clear_sprite_2+1
    lda tab_clear_sprite_2_high,y
    sta ?clear_sprite_2+2

    ldy nr_sprite
    ldx block_y                   ; Y position in screen
    lda #0
?clear_sprite_2
    jmp cls0c



?shape_3
    lda @all_sprite_data_table_low,x
    sta data
    lda @all_sprite_data_table_high,x
    sta data+1

    sty nr_sprite
    ldx block_y
?set_sprite_3
    jsr player3a

    ldy block_dy
    lda tab_clear_sprite_3_low,y
    sta ?clear_sprite_3+1
    lda tab_clear_sprite_3_high,y
    sta ?clear_sprite_3+2

    ldy nr_sprite
    ldx block_y
    lda #0
?clear_sprite_3
    jmp cls0d


; very fast clear functions due to unroll loops


tab_clear_sprite_0_low    .byte <cls0,<cls1,<cls2,<cls3,<cls4,<cls5,<cls6,<cls7
tab_clear_sprite_0_high    .byte >cls0,>cls1,>cls2,>cls3,>cls4,>cls5,>cls6,>cls7


cls0
    sta pmadr+$400+$10,x

    sta pmadr+$400-8,x
    sta pmadr+$400-7,x
    sta pmadr+$400-6,x
    sta pmadr+$400-5,x
    sta pmadr+$400-4,x
    sta pmadr+$400-3,x
    sta pmadr+$400-2,x
    sta pmadr+$400-1,x


    rts

cls1
    sta pmadr+$400+$13,x
    sta pmadr+$400+$14,x
    sta pmadr+$400+$15,x
    sta pmadr+$400+$16,x
    sta pmadr+$400+$17,x
    sta pmadr+$400+$18,x
    jmp cls7_36


cls2
    sta pmadr+$400+$13,x
    sta pmadr+$400+$14,x
    sta pmadr+$400+$15,x
    sta pmadr+$400+$16,x
    sta pmadr+$400+$17,x
    jmp cls7_30

cls3
    sta pmadr+$400+$13,x
    sta pmadr+$400+$14,x
    sta pmadr+$400+$15,x
    sta pmadr+$400+$16,x
    jmp cls7_24

cls4
    sta pmadr+$400+$13,x
    sta pmadr+$400+$14,x
    sta pmadr+$400+$15,x
    jmp cls7_18


cls5
;    cpx #POSY_MAX-32
;    bcs cls7_12
    sta pmadr+$400+$13,x
    sta pmadr+$400+$14,x
    jmp cls7_12

cls6
    sta pmadr+$400+$13,x
    jmp cls7_6


cls7
    sta pmadr+$400-7,x
cls7_6
    sta pmadr+$400-6,x
cls7_12
    sta pmadr+$400-5,x
cls7_18
    sta pmadr+$400-4,x
cls7_24
    sta pmadr+$400-3,x
cls7_30
    sta pmadr+$400-2,x
cls7_36
    sta pmadr+$400-1,x

    sta pmadr+$400+$10,x
    sta pmadr+$400+$11,x
    sta pmadr+$400+$12,x
    rts


tab_clear_sprite_1_low    .byte <cls0b,<cls1b,<cls2b,<cls3b,<cls4b,<cls5b,<cls6b,<cls7b
tab_clear_sprite_1_high    .byte >cls0b,>cls1b,>cls2b,>cls3b,>cls4b,>cls5b,>cls6b,>cls7b


cls0b
    sta pmadr+$500+$11,x

    sta pmadr+$500+$10,x

    sta pmadr+$500-8,x
    sta pmadr+$500-7,x
    sta pmadr+$500-6,x
    sta pmadr+$500-5,x
    sta pmadr+$500-4,x
    sta pmadr+$500-3,x
    sta pmadr+$500-2,x
    sta pmadr+$500-1,x


    rts

cls1b
    sta pmadr+$500+$13,x
    sta pmadr+$500+$14,x
    sta pmadr+$500+$15,x
    sta pmadr+$500+$16,x
    sta pmadr+$500+$17,x
    sta pmadr+$500+$18,x
    jmp cls7b_36


cls2b
    sta pmadr+$500+$13,x
    sta pmadr+$500+$14,x
    sta pmadr+$500+$15,x
    sta pmadr+$500+$16,x
    sta pmadr+$500+$17,x
    jmp cls7b_30

cls3b
    sta pmadr+$500+$13,x
    sta pmadr+$500+$14,x
    sta pmadr+$500+$15,x
    sta pmadr+$500+$16,x
    jmp cls7b_24

cls4b
    sta pmadr+$500+$13,x
    sta pmadr+$500+$14,x
    sta pmadr+$500+$15,x
    jmp cls7b_18

cls5b
;    cpx #POSY_MAX-32
;    bcs cls7b_12
    sta pmadr+$500+$13,x
    sta pmadr+$500+$14,x
    jmp cls7b_12


cls6b
    sta pmadr+$500+$13,x
    jmp cls7b_6

cls7b
    sta pmadr+$500-7,x
cls7b_6
    sta pmadr+$500-6,x
cls7b_12
    sta pmadr+$500-5,x
cls7b_18
    sta pmadr+$500-4,x
cls7b_24
    sta pmadr+$500-3,x
cls7b_30
    sta pmadr+$500-2,x
cls7b_36
    sta pmadr+$500-1,x

    sta pmadr+$500+$10,x
    sta pmadr+$500+$11,x
    sta pmadr+$500+$12,x
    rts


tab_clear_sprite_2_low    .byte <cls0c,<cls1c,<cls2c,<cls3c,<cls4c,<cls5c,<cls6c,<cls7c
tab_clear_sprite_2_high    .byte >cls0c,>cls1c,>cls2c,>cls3c,>cls4c,>cls5c,>cls6c,>cls7c


cls0c
    sta pmadr+$600+$11,x

    sta pmadr+$600+$10,x

    sta pmadr+$600-8,x
    sta pmadr+$600-7,x
    sta pmadr+$600-6,x
    sta pmadr+$600-5,x
    sta pmadr+$600-4,x
    sta pmadr+$600-3,x
    sta pmadr+$600-2,x
    sta pmadr+$600-1,x


    rts

cls1c
    sta pmadr+$600+$13,x
    sta pmadr+$600+$14,x
    sta pmadr+$600+$15,x
    sta pmadr+$600+$16,x
    sta pmadr+$600+$17,x
    sta pmadr+$600+$18,x
    jmp cls7c_36


cls2c
    sta pmadr+$600+$13,x
    sta pmadr+$600+$14,x
    sta pmadr+$600+$15,x
    sta pmadr+$600+$16,x
    sta pmadr+$600+$17,x
    jmp cls7c_30

cls3c
    sta pmadr+$600+$13,x
    sta pmadr+$600+$14,x
    sta pmadr+$600+$15,x
    sta pmadr+$600+$16,x
    jmp cls7c_24

cls4c
    sta pmadr+$600+$13,x
    sta pmadr+$600+$14,x
    sta pmadr+$600+$15,x
    jmp cls7c_18

cls5c
;    cpx #POSY_MAX-32
;    ccs cls7c_12
    sta pmadr+$600+$13,x
    sta pmadr+$600+$14,x
    jmp cls7c_12


cls6c
    sta pmadr+$600+$13,x
    jmp cls7c_6

cls7c
    sta pmadr+$600-7,x
cls7c_6
    sta pmadr+$600-6,x
cls7c_12
    sta pmadr+$600-5,x
cls7c_18
    sta pmadr+$600-4,x
cls7c_24
    sta pmadr+$600-3,x
cls7c_30
    sta pmadr+$600-2,x
cls7c_36
    sta pmadr+$600-1,x

    sta pmadr+$600+$10,x
    sta pmadr+$600+$11,x
    sta pmadr+$600+$12,x
    rts

tab_clear_sprite_3_low    .byte <cls0d,<cls1d,<cls2d,<cls3d,<cls4d,<cls5d,<cls6d,<cls7d
tab_clear_sprite_3_high    .byte >cls0d,>cls1d,>cls2d,>cls3d,>cls4d,>cls5d,>cls6d,>cls7d


cls0d
    sta pmadr+$700+$11,x

    sta pmadr+$700+$10,x

    sta pmadr+$700-8,x
    sta pmadr+$700-7,x
    sta pmadr+$700-6,x
    sta pmadr+$700-5,x
    sta pmadr+$700-4,x
    sta pmadr+$700-3,x
    sta pmadr+$700-2,x
    sta pmadr+$700-1,x


    rts

cls1d
    sta pmadr+$700+$13,x
    sta pmadr+$700+$14,x
    sta pmadr+$700+$15,x
    sta pmadr+$700+$16,x
    sta pmadr+$700+$17,x
    sta pmadr+$700+$18,x
    jmp cls7d_36


cls2d
    sta pmadr+$700+$13,x
    sta pmadr+$700+$14,x
    sta pmadr+$700+$15,x
    sta pmadr+$700+$16,x
    sta pmadr+$700+$17,x
    jmp cls7d_30

cls3d
    sta pmadr+$700+$13,x
    sta pmadr+$700+$14,x
    sta pmadr+$700+$15,x
    sta pmadr+$700+$16,x
    jmp cls7d_24

cls4d
    sta pmadr+$700+$13,x
    sta pmadr+$700+$14,x
    sta pmadr+$700+$15,x
    jmp cls7d_18

cls5d
;    cpx #POSY_MAX-32
;    ccs cls7d_12
    sta pmadr+$700+$13,x
    sta pmadr+$700+$14,x
    jmp cls7d_12


cls6d
    sta pmadr+$700+$13,x
    jmp cls7d_6

cls7d
    sta pmadr+$700-7,x
cls7d_6
    sta pmadr+$700-6,x
cls7d_12
    sta pmadr+$700-5,x
cls7d_18
    sta pmadr+$700-4,x
cls7d_24
    sta pmadr+$700-3,x
cls7d_30
    sta pmadr+$700-2,x
cls7d_36
    sta pmadr+$700-1,x

    sta pmadr+$700+$10,x
    sta pmadr+$700+$11,x
    sta pmadr+$700+$12,x
    rts

; Very fast data copy functions due to unroll loops

player0a
 ldy #0                ; 2
 lda (data),y          ; 5
 sta pmadr+$400+0,x    ; 5
 iny                   ; 2
 lda (data),y
 sta pmadr+$400+1,x
 iny
 lda (data),y
 sta pmadr+$400+2,x
 iny
 lda (data),y
 sta pmadr+$400+3,x
 iny
 lda (data),y
 sta pmadr+$400+4,x
 iny
 lda (data),y
 sta pmadr+$400+5,x
 iny
 lda (data),y
 sta pmadr+$400+6,x
 iny
 lda (data),y
 sta pmadr+$400+7,x
 iny
 lda (data),y
 sta pmadr+$400+8,x
 iny
 lda (data),y
 sta pmadr+$400+9,x
 iny
 lda (data),y
 sta pmadr+$400+10,x
 iny
 lda (data),y
 sta pmadr+$400+11,x
 iny
 lda (data),y
 sta pmadr+$400+12,x
 iny
 lda (data),y
 sta pmadr+$400+13,x
 iny
 lda (data),y
 sta pmadr+$400+14,x
 iny
 lda (data),y
 sta pmadr+$400+15,x
; iny                  ; sum:12*16 = 192 cycles
 rts

player1a
 ldy #0                ; 2
 lda (data),y          ; 5
 sta pmadr+$500+0,x    ; 5
 iny                   ; 2
 lda (data),y
 sta pmadr+$500+1,x
 iny
 lda (data),y
 sta pmadr+$500+2,x
 iny
 lda (data),y
 sta pmadr+$500+3,x
 iny
 lda (data),y
 sta pmadr+$500+4,x
 iny
 lda (data),y
 sta pmadr+$500+5,x
 iny
 lda (data),y
 sta pmadr+$500+6,x
 iny
 lda (data),y
 sta pmadr+$500+7,x
 iny
 lda (data),y
 sta pmadr+$500+8,x
 iny
 lda (data),y
 sta pmadr+$500+9,x
 iny
 lda (data),y
 sta pmadr+$500+10,x
 iny
 lda (data),y
 sta pmadr+$500+11,x
 iny
 lda (data),y
 sta pmadr+$500+12,x
 iny
 lda (data),y
 sta pmadr+$500+13,x
 iny
 lda (data),y
 sta pmadr+$500+14,x
 iny
 lda (data),y
 sta pmadr+$500+15,x
 rts

player2a
 ldy #0                ; 2
 lda (data),y          ; 5
 sta pmadr+$600+0,x    ; 5
 iny                   ; 2
 lda (data),y
 sta pmadr+$600+1,x
 iny
 lda (data),y
 sta pmadr+$600+2,x
 iny
 lda (data),y
 sta pmadr+$600+3,x
 iny
 lda (data),y
 sta pmadr+$600+4,x
 iny
 lda (data),y
 sta pmadr+$600+5,x
 iny
 lda (data),y
 sta pmadr+$600+6,x
 iny
 lda (data),y
 sta pmadr+$600+7,x
 iny
 lda (data),y
 sta pmadr+$600+8,x
 iny
 lda (data),y
 sta pmadr+$600+9,x
 iny
 lda (data),y
 sta pmadr+$600+10,x
 iny
 lda (data),y
 sta pmadr+$600+11,x
 iny
 lda (data),y
 sta pmadr+$600+12,x
 iny
 lda (data),y
 sta pmadr+$600+13,x
 iny
 lda (data),y
 sta pmadr+$600+14,x
 iny
 lda (data),y
 sta pmadr+$600+15,x
 rts

player3a
 ldy #0                ; 2
 lda (data),y          ; 5
 sta pmadr+$700+0,x    ; 5
 iny                   ; 2
 lda (data),y
 sta pmadr+$700+1,x
 iny
 lda (data),y
 sta pmadr+$700+2,x
 iny
 lda (data),y
 sta pmadr+$700+3,x
 iny
 lda (data),y
 sta pmadr+$700+4,x
 iny
 lda (data),y
 sta pmadr+$700+5,x
 iny
 lda (data),y
 sta pmadr+$700+6,x
 iny
 lda (data),y
 sta pmadr+$700+7,x
 iny
 lda (data),y
 sta pmadr+$700+8,x
 iny
 lda (data),y
 sta pmadr+$700+9,x
 iny
 lda (data),y
 sta pmadr+$700+10,x
 iny
 lda (data),y
 sta pmadr+$700+11,x
 iny
 lda (data),y
 sta pmadr+$700+12,x
 iny
 lda (data),y
 sta pmadr+$700+13,x
 iny
 lda (data),y
 sta pmadr+$700+14,x
 iny
 lda (data),y
 sta pmadr+$700+15,x
 rts

