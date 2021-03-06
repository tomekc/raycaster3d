;北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
;
;  32-bit column drawing; code "borrowed" from ACK/386
;
;  compiler: WATCOM/DJGPP
;
;  original code: Jaimi McEntire
;  GCC port:      Tomasz Cejner
;
;  Niniejszy plik zawiera procedur� rysuj哻� pionowe, przeskalowane,
;  obci憈e w pione paski tekstury. Tekstury musz� by� zapisane kolumnami
;  od gy, wi慶 b慸� wymaga抷 wcze瀗iej przetworzenia (np. programem OBRAC).
;
;  Procedura ta mo rysowa� pionowe paski na ekranie o dowolnych parametrach.
;  Wystarczy zmieni� ustawienia funkcj� DC_ScreenSetup() - podajemy adres
;  ekranu, kty mo wskazywa� gdziekolwiek (sic!).
;
;  Procedura DC_TSliver rysuje paski z kolorami 0 jako "przezroczystymi".
;  Zdecydowa抏m si� na rozdzielenie, mimo i�, gdyby u� jej do rysowania
;  瀋ian strata pr慸ko瀋i by抋 by niewielka.
;
;北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
.486P
.MODEL FLAT,C

SHADE equ 1

;北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
.DATA
;北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
DCwidth         dd 320                  ; domyslne wartosci
DCht            dd 100
DCscrn          dd 0a0000h
DCPal           dd 0
DClight         dd 0
DChoff          dd 32000
DCangle         dd 0                    ; local

;北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
.CODE
;北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北

;鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍
public DC_ScreenSetup
DC_ScreenSetup proc wt:Dword, ht:dword, scrn:dword
        pushad
        mov eax,[wt]
        mov [DCwidth],eax
        mov ebx,[ht]
        mov [DCht],ebx
        mul ebx
        mov [DChoff],eax
        mov eax,[scrn]
        mov [DCscrn],eax
        popad
        ret
DC_ScreenSetup endp
;鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍
;  Tej funkcji podajemy [pal] - wskaik na tablic� cieniowania.
;  Tablica cieniowania zawiera zestawy 256 indeks do kolor o coraz to
;  nizej intensywno瀋i. [licht] = 0 wy拞cza 瀋iemnianie z odleg抩瀋i�.
;鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍
public DC_LightSetup
DC_LightSetup proc pal:dword, licht:dword
        push eax
        mov eax,[pal]
        mov [DCPal],eax
        mov eax,[licht]
        mov [DClight],eax
        pop eax
        ret
DC_LightSetup endp
;鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍
; DC_Sliver (int Col,int slice,int dist,int width,int ht,UCHAR far *Wall,
;	    UCHAR far *Screen,UCHAR far *Pal,int light,int offset);
; Contributed by Jaimi McEntire
;鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍
	db 'HEREWEGO:SLIVER'
public DC_Sliver
DC_Sliver proc DCcol:dword, DCslice:dword, DCdist:dword, DCwall:dword
        pushad

        mov word ptr DCangle,0
        mov edx,0
        mov esi,0

        mov eax,DCdist        ; getedistance to object
        shr eax,6             ; divide it by 64 to get zones.

        cmp eax,15            ; make sure zone doesn't go beyond 15
        jl zoneokay
        mov eax,15

zoneokay:
; Good place for light medifier to medify zone
        shl eax,8                ; mult by 256 to get offset into paltables.
        mov edx,DCPal            ; point EDX at PalTables
        cmp word ptr DClight,0
        jz zonelight
        add edx,eax              ; EDX points to corrected palette for zone.

zonelight:
        mov edi,DCscrn ; ESeDI point to area we are painting

;/       moveax,column
;/       cmpeax,ViewX
;/	 jl  outtahere		 ; don't bother to draw a column we wont see.
;/       cmpeax,ViewX2
;/	 jge outtahere
;/       subeax,ViewX
;/       mov columneax           ; adjust column for offset into buffer.

        mov esi,DCwall        ; ESI point to wall buffer

;;;     mov    ebx,DCht          ; center row.
;;;     imul   ebx,word ptr DCwidth
        mov    ebx,DChoff        ; Pick up offset to center of viewport
        add    ebx,DCcol
        add    edi,ebx           ; edi now point at starting area (top run)
        mov    ebx,edi
        add    ebx,DCwidth       ; ebx points to 1 down (for bottom run)

        mov    ecx,0             ; ht.
        mov    eax,DCslice
        shl    eax,6             ; mult slice by 64
        add    eax,esi           ; add in offset to wall bitmap
        mov    DCslice,eax       ; save it
        mov    eax,0

looptop:
        movzx  eax,ch            ; current ht
        mov    esi,31            ; base to start with
        sub    esi,eax           ; base - ht = row
        add    esi,DCslice       ; plus column start to point at the pixel.
        mov     al,[esi]         ; mov al,offset slice[esi]
IFDEF TRANSPARENT
	or	al,al		 ; see if transparent
	jz	blank		 ; yes, don't draw this pixel
ENDIF
IFDEF SHADE
        mov     al,[edx][eax]    ; added this line to get correct sheding.
ENDIF
        mov     [edi],al
blank:
        movzx  esi,ch           ; use a cool 386 instruction to accomplish
        add    esi,32           ; same as mov eax,ecx; shr eax,8 !!
        add    esi,DCslice      ; add in starting column
        mov     al,[esi]
IFDEF TRANSPARENT
	or	al,al
	je	blank2
ENDIF
IFDEF SHADE
        mov     al,[edx][eax]         ; added this line to shade it.
ENDIF
        mov     [ebx],al

blank2:
        mov    eax,DCwidth
        sub    edi,eax                   ; point to the next two dest pixels.
        add    ebx,eax                   ; point to the next two dest pixels.
        add    ecx,DCdist                ; addedistance to ht adjuster.

        cmp     ch,32                    ; cmp to 32 (1 half our bmesize)
	jge	outtahere		 ; get out if bitmap done.

;;;;	inc	word ptr DCangle
        mov    eax,DCangle
        inc    eax
        cmp    eax,DCht                  ; if half height,
	jge	outtahere		 ; then leave.
        mov     DCangle,eax
	jmp	looptop

outtahere:

        popad
	ret
DC_Sliver endp
;鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍
; DC_TSliver (int Col,int slice,int dist,int width,int ht,UCHAR far *Wall,
;	    UCHAR far *Screen,UCHAR far *Pal,int light,int offset);
; Contributed by Jaimi McEntire
; transparent sliver - zero pixels are ignored
;鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍鞍
public DC_TSliver
DC_TSliver proc DCcol:dword, DCslice:dword, DCdist:dword, DCwall:dword
        pushad

        mov word ptr DCangle,0
        mov edx,0
        mov esi,0

        mov eax,DCdist        ; getedistance to object
        shr eax,6             ; divide it by 64 to get zones.

        cmp eax,15            ; make sure zone doesn't go beyond 15
        jl zoneokay
        mov eax,15

zoneokay:
; Good place for light medifier to medify zone
        shl eax,8                ; mult by 256 to get offset into paltables.
        mov edx,DCPal            ; point EDX at PalTables
        cmp word ptr DClight,0
        jz zonelight
        add edx,eax              ; EDX points to corrected palette for zone.

zonelight:
        mov    edi,DCscrn        ; ESeDI point to area we are painting
        mov    esi,DCwall        ; ESI point to wall buffer
        mov    ebx,DChoff        ; Pick up offset to center of viewport
        add    ebx,DCcol
        add    edi,ebx           ; edi now point at starting area (top run)
        mov    ebx,edi
        add    ebx,DCwidth       ; ebx points to 1 down (for bottom run)

        mov    ecx,0             ; ht.
        mov    eax,DCslice
        shl    eax,6             ; mult slice by 64
        add    eax,esi           ; add in offset to wall bitmap
        mov    DCslice,eax       ; save it
        mov    eax,0

looptop:
        movzx  eax,ch            ; current ht
        mov    esi,31            ; base to start with
        sub    esi,eax           ; base - ht = row
        add    esi,DCslice       ; plus column start to point at the pixel.
        mov     al,[esi]         ; mov al,offset slice[esi]
	or	al,al		 ; see if transparent
	jz	blank		 ; yes, don't draw this pixel
IFDEF SHADE
        mov     al,[edx][eax]    ; added this line to get correct sheding.
ENDIF
        mov     [edi],al
blank:
        movzx  esi,ch           ; use a cool 386 instruction to accomplish
        add    esi,32           ; same as mov eax,ecx; shr eax,8 !!
        add    esi,DCslice      ; add in starting column
        mov     al,[esi]
	or	al,al
	je	blank2
IFDEF SHADE
        mov     al,[edx][eax]         ; added this line to shade it.
ENDIF
        mov     [ebx],al

blank2:
        mov    eax,DCwidth
        sub    edi,eax                   ; point to the next two dest pixels.
        add    ebx,eax                   ; point to the next two dest pixels.
        add    ecx,DCdist                ; addedistance to ht adjuster.

        cmp     ch,32                    ; cmp to 32 (1 half our bmesize)
	jge	outtahere		 ; get out if bitmap done.

        mov    eax,DCangle
        inc    eax
        cmp    eax,DCht                  ; if half height,
	jge	outtahere		 ; then leave.
        mov     DCangle,eax
	jmp	looptop

outtahere:

        popad
	ret
DC_TSliver endp



	END

