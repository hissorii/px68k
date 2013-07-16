;-----------------------------------------------------------------------------
;   Memory & I/O Access Routine for WinX68k
;-----------------------------------------------------------------------------

	BITS 32

	SECTION .data

%define	_MEM		MEM
%define	_IPL		IPL
%define	_OP_ROM		OP_ROM
%define	_BusErrFlag	BusErrFlag
%define	_BusErrAdr	BusErrAdr
%define	_AdrErrAdr	AdrErrAdr
%define	_MemByteAccess	MemByteAccess
%define	AdrError	@AdrError@8
%define	BusError	@BusError@8

%define	_FONT		FONT
%define	_GVRAM		GVRAM
%define	_TVRAM		TVRAM
%define	_SRAM		SRAM
%define	_SCSIIPL	SCSIIPL

	global	_MEM
	global	_IPL
	global	_OP_ROM
	global	_FONT
	global	_BusErrFlag
	global	_BusErrAdr
	global	_AdrErrAdr
	global	_MemByteAccess

	extern	_GVRAM
	extern	_TVRAM
	extern	_SRAM
	extern	_SCSIIPL

	SECTION .text

;-----------------------------------------------------------------------------

	extern	BusError
	extern	AdrError

	extern	@VCtrl_Read@4
	extern	@CRTC_Read@4
	extern	@GVRAM_Read@4
	extern	@TVRAM_Read@4
	extern	@BG_Read@4
	extern	@Pal_Read@4
	extern	@DMA_Read@4
	extern	@MFP_Read@4
	extern	@RTC_Read@4
	extern	@OPM_Read@4
	extern	@ADPCM_Read@4
	extern	@MIDI_Read@4
	extern	@FDC_Read@4
	extern	@SASI_Read@4
	extern	@SCC_Read@4
	extern	@PIA_Read@4
	extern	@IOC_Read@4
	extern	@SRAM_Read@4
	extern	@SysPort_Read@4
	extern	@WinDrv_Read@4
	extern	@Mcry_Read@4
	extern	@SCSI_Read@4

	extern	@VCtrl_Write@8
	extern	@CRTC_Write@8
	extern	@GVRAM_Write@8
	extern	@TVRAM_Write@8
	extern	@BG_Write@8
	extern	@Pal_Write@8
	extern	@DMA_Write@8
	extern	@MFP_Write@8
	extern	@RTC_Write@8
	extern	@OPM_Write@8
	extern	@ADPCM_Write@8
	extern	@MIDI_Write@8
	extern	@FDC_Write@8
	extern	@SASI_Write@8
	extern	@SCC_Write@8
	extern	@PIA_Write@8
	extern	@IOC_Write@8
	extern	@SRAM_Write@8
	extern	@SysPort_Write@8
	extern	@WinDrv_Write@8
	extern	@Mcry_Write@8
	extern	@SCSI_Write@8

	extern	@Memory_ErrTrace@0

; 公開I/F
	global  @cpu_readmem24@4
	global  @cpu_readmem24_word@4
	global  @cpu_readmem24_dword@4
	global  @cpu_writemem24@8
	global  @cpu_writemem24_word@8
	global  @cpu_writemem24_dword@8

	global  @dma_readmem24@4
	global  @dma_readmem24_word@4
	global  @dma_readmem24_dword@4
	global  @dma_writemem24@8
	global  @dma_writemem24_word@8
	global  @dma_writemem24_dword@8

	global  @cpu_setOPbase24@4

	global  @Memory_SetSCSIMode@0

;-----------------------------------------------------------------------------
;  Memory Write
;  後々のために、BusErrとAdrErrはReadMemとは別ルーチンを用意しておこう…

	ALIGN 4

@dma_writemem24@8:
;	mov	byte [_MemByteAccess], 1	; 神戸恋愛物語
	call	wm_main
	mov	byte [_MemByteAccess], 0
	ret
@dma_writemem24_word@8:
	mov	byte [_MemByteAccess], 0
	test	cl, 1			; 奇数バウンダリはアドレスエラー
	jnz	dmaw_jmpae
	mov	byte [WriteValue], dl	; 下位バイト退避
	shr	dx, 8			; 上位バイト書きこみ
	push	ecx
	call	wm_main
	pop	ecx
	inc	ecx
	mov	dl, byte [WriteValue]	; 下位バイト書きこみ
	call	wm_main
	ret
@dma_writemem24_dword@8:
	mov	byte [_MemByteAccess], 0
	test	cl, 1			; 奇数バウンダリはアドレスエラー
	jnz	dmaw_jmpae
	mov	dword [WriteValue], edx	; 書きこみデータ退避
	shr	edx, 24
	push	ecx
	call	wm_main
	pop	ecx
	inc	ecx
	mov	dl, byte [WriteValue+2]
	push	ecx
	call	wm_main
	pop	ecx
	inc	ecx
	mov	dl, byte [WriteValue+1]
	push	ecx
	call	wm_main
	pop	ecx
	inc	ecx
	mov	dl, byte [WriteValue]
	call	wm_main
	ret
dmaw_jmpae:
	mov	byte [_BusErrFlag], 4
	ret


@cpu_writemem24@8:
	mov	byte [_BusErrFlag], 0
;	mov	byte [_MemByteAccess], 1	; ZOUNDS
	mov	byte [_MemByteAccess], 0
	call	wm_cnt			; バスエラーチェックはいらない
;	mov	byte [_MemByteAccess], 0
	test	byte [_BusErrFlag], 2
	jnz	near wm_jmpbe
	ret
@cpu_writemem24_word@8:
	mov	byte [_MemByteAccess], 0
	test	cl, 1			; 奇数バウンダリはアドレスエラー
	jnz	near wm_jmpae
	mov	byte [_BusErrFlag], 0
	mov	byte [WriteValue], dl	; 下位バイト退避
	shr	dx, 8			; 上位バイト書きこみ
	push	ecx
	call	wm_cnt			; バスエラーチェックはいらない
	pop	ecx
	inc	ecx
	mov	dl, byte [WriteValue]	; 下位バイト書きこみ
	call	wm_main
	test	byte [_BusErrFlag], 2
	jnz	wm_jmpbe
	ret
@cpu_writemem24_dword@8:
	mov	byte [_MemByteAccess], 0
	test	cl, 1			; 奇数バウンダリはアドレスエラー
	jnz	wm_jmpae
	mov	byte [_BusErrFlag], 0
	mov	dword [WriteValue], edx	; 書きこみデータ退避
	shr	edx, 24
	push	ecx
	call	wm_cnt			; バスエラーチェックはいらない
	pop	ecx
	inc	ecx
	mov	dl, byte [WriteValue+2]
	push	ecx
	call	wm_main
	pop	ecx
	inc	ecx
	mov	dl, byte [WriteValue+1]
	push	ecx
	call	wm_main
	pop	ecx
	inc	ecx
	mov	dl, byte [WriteValue]
	call	wm_main
	test	byte [_BusErrFlag], 2
	jnz	wm_jmpbe
	ret

wm_jmpae:
	jmp	AdrError
wm_jmpbe:
	call	@Memory_ErrTrace@0
	jmp	BusError

wm_main:
	test	byte [_BusErrFlag], 7
	jz	wm_cnt
	ret				; 既にバスエラーが起こってたら何もしない（いちおー、ね…）
wm_cnt:
	and	edx, 0ffh
	and	ecx, 0ffffffh
	cmp	ecx, 0a00000h		; Main Memory 10Mb
;	cmp	ecx, 0c00000h		; Main Memory 12Mb
	jc	wm_ram
	cmp	ecx, 0c00000h		; 
	jc	wm_buserr
	cmp	ecx, 0e00000h		; Graphic VRAM
	jc	wm_gvram
	mov	eax, ecx
	shr	eax, 11
	and	eax, 03fch
	jmp	[MemWriteTable+eax]

wm_gvram:
	jmp	@GVRAM_Write@8

wm_ram:
	xor	cl, 1
	mov	eax, dword [_MEM]
	mov	byte [ecx+eax], dl
	ret

wm_opm:
	and	cl, 3
	cmp	cl, 1
	jz	wm_opmreg
	cmp	cl, 3
	jnz	wm_nop
	mov	ecx, 1
	jmp	@OPM_Write@8
wm_opmreg:
	xor	ecx, ecx
	jmp	@OPM_Write@8

wm_e82_pal:
	jmp	@Pal_Write@8
wm_e82_vctrl:
	jmp	@VCtrl_Write@8
wm_e82:
	cmp	ecx, 0e82400h
	jc	wm_e82_pal
	cmp	ecx, 0e82700h
	jc	wm_e82_vctrl
	ret			; バスエラーにしない方がいい？（イメージファイトでこの辺のどっか読み込んでる）

wm_buserr:
	mov	byte [_BusErrFlag], 2
	mov	dword [_BusErrAdr], ecx
	ret

wm_nop:
	ret

;-----------------------------------------------------------------------------
;  Memory Read
;
	ALIGN 4

@dma_readmem24@4:
	call	rm_main
	ret
@dma_readmem24_word@4:
	test	cl, 1
	jnz	dmar_jmpae
	push	ecx
	call	rm_main
	pop	ecx
	inc	ecx
	push	ax
	call	rm_main
	pop	cx
	mov	ah, cl
	ret
@dma_readmem24_dword@4:
	test	cl, 1
	jnz	dmar_jmpae
	push	ecx
	call	rm_main
	pop	ecx
	inc	ecx
	mov	byte [RetValue+3], al
	push	ecx
	call	rm_main
	pop	ecx
	inc	ecx
	mov	byte [RetValue+2], al
	push	ecx
	call	rm_main
	pop	ecx
	inc	ecx
	mov	byte [RetValue+1], al
	call	rm_main
	mov	byte [RetValue], al
	mov	eax, dword [RetValue]
	ret
dmar_jmpae:
	mov	byte [_BusErrFlag], 3
	ret


@cpu_readmem24@4:
	call	rm_main
	test	byte [_BusErrFlag], 1
	jnz	near rm_jmpbe
	ret
@cpu_readmem24_word@4:
	test	cl, 1
	jnz	rm_jmpae
	mov	byte [_BusErrFlag], 0
	push	ecx
	call	rm_main
	pop	ecx
	inc	ecx
	push	ax
	call	rm_main
	pop	cx
	test	byte [_BusErrFlag], 1
	jnz	rm_jmpbe
	mov	ah, cl
	ret
@cpu_readmem24_dword@4:
	mov	byte [_MemByteAccess], 0
	test	cl, 1
	jnz	rm_jmpae
	mov	byte [_BusErrFlag], 0
	push	ecx
	call	rm_main
	pop	ecx
	inc	ecx
	mov	byte [RetValue+3], al
	push	ecx
	call	rm_main
	pop	ecx
	inc	ecx
	mov	byte [RetValue+2], al
	push	ecx
	call	rm_main
	pop	ecx
	inc	ecx
	mov	byte [RetValue+1], al
	call	rm_main
	test	byte [_BusErrFlag], 1
	jnz	rm_jmpbe
	mov	byte [RetValue], al
	mov	eax, dword [RetValue]
	ret

rm_jmpae:
	jmp	AdrError
rm_jmpbe:
	call	@Memory_ErrTrace@0
	jmp	BusError

rm_main:
	and	ecx, 0ffffffh
	cmp	ecx, 0a00000h		; Main Memory 10Mb
;	cmp	ecx, 0c00000h		; Main Memory 12Mb
	jc	rm_ram
	cmp	ecx, 0c00000h		; 
	jc	rm_buserr
	cmp	ecx, 0e00000h		; Graphic VRAM
	jc	rm_gvram
	mov	eax, ecx
	shr	eax, 11
	and	eax, 03fch
	jmp	[MemReadTable+eax]

rm_gvram:
	jmp	@GVRAM_Read@4

rm_ram:
	xor	cl, 1
	mov	eax, dword [_MEM]
	mov	al, byte [ecx+eax]
	ret
rm_font:
	and	ecx, 0fffffh
	mov	eax, dword [_FONT]
	mov	al, byte [ecx+eax]
	ret
rm_ipl:
	and	ecx, 03ffffh
	xor	cl, 1
	mov	eax, dword [_IPL]
	mov	al, byte [ecx+eax]
	ret

rm_nop:
	xor	al, al
;	mov	al, 0ffh		; Genocide2 → SysPortで対処
	ret

rm_opm:
	and	cl, 3
	cmp	cl, 3
	jne	rm_nop
	xor	ecx, ecx
	jmp	@OPM_Read@4

rm_e82_pal:
	jmp	@Pal_Read@4
rm_e82_vctrl:
	jmp	@VCtrl_Read@4
rm_e82:
	cmp	ecx, 0e82400h
	jc	rm_e82_pal
	cmp	ecx, 0e82700h
;	cmp	ecx, 0e83000h
	jc	rm_e82_vctrl

	xor	al, al			; いめーじふぁいと。バスエラーにしたら死ぬみたい
	ret

rm_buserr:
	mov	byte [_BusErrFlag], 1
	mov	dword [_BusErrAdr], ecx
	ret


;-----------------------------------------------------------------------------
;  Memory Operation Base Set
;  マクロにして68KEMに組み込む方が正解かなぁ…

@cpu_setOPbase24@4:
	and	ecx, 0ffffffh
	mov	eax, ecx
	shr	eax, 20
	and	eax, 0fh
	jmp	[OPBaseTable+eax*4]
setop_mem:
	mov	eax, dword [_MEM]
	mov	dword [_OP_ROM], eax
	ret
setop_gvr:
	mov	eax, _GVRAM
	sub	eax, 0c00000h
	mov	dword [_OP_ROM], eax
	ret
setop_tvr:
	cmp	ecx, 0e80000h
	jc	setop_tvram
	cmp	ecx, 0ea0000h
	jc	setop_jmpbe
	cmp	ecx, 0ea2000h
	jc	setop_scsi
	cmp	ecx, 0ed0000h
	jc	setop_jmpbe
	cmp	ecx, 0ed4000h
	jnc	setop_jmpbe
	mov	eax, _SRAM
	sub	eax, 0ed0000h
	mov	dword [_OP_ROM], eax
	ret
setop_tvram:
	mov	eax, _TVRAM
	sub	eax, 0e00000h
	mov	dword [_OP_ROM], eax
	ret

setop_scsi:
	mov	eax, _SCSIIPL
	cmp	ecx, 0ea2000h
	jnc	setop_jmpbe
	sub	eax, 0ea0000h
	mov	dword [_OP_ROM], eax
	ret

setop_rom:
	cmp	ecx, 0fc0000h
	jc	near rm_jmpbe
	mov	eax, dword [_IPL]
	sub	eax, 0fc0000h
	mov	dword [_OP_ROM], eax
	ret
setop_jmpbe:
	mov	byte [_BusErrFlag], 3
	mov	dword [_BusErrAdr], ecx
	call	@Memory_ErrTrace@0
	jmp	BusError


@Memory_SetSCSIMode@0:
	mov	eax, rm_buserr
	mov	ebx, 380h
	mov	ecx, 16
mem030loop:
	mov	dword [MemReadTable+ebx], eax
	add	ebx, 4
	dec	ecx
	jne	mem030loop
	ret


;-----------------------------------------------------------------------------
;  てーぶる類
;
	SECTION .data

	ALIGN 4

MemReadTable:
		dd	@TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4
		dd	@TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4
		dd	@TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4
		dd	@TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4
		dd	@TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4
		dd	@TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4
		dd	@TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4
		dd	@TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4, @TVRAM_Read@4
		dd	@CRTC_Read@4, rm_e82, @DMA_Read@4, rm_nop, @MFP_Read@4, @RTC_Read@4, rm_nop, @SysPort_Read@4
		dd	rm_opm, @ADPCM_Read@4, @FDC_Read@4, @SASI_Read@4, @SCC_Read@4, @PIA_Read@4, @IOC_Read@4, @WinDrv_Read@4;rm_nop
		dd	@SCSI_Read@4, rm_buserr, rm_buserr, rm_buserr, rm_buserr, rm_buserr, rm_buserr, @MIDI_Read@4
		dd	@BG_Read@4, @BG_Read@4, @BG_Read@4, @BG_Read@4, @BG_Read@4, @BG_Read@4, @BG_Read@4, @BG_Read@4
%ifndef	NO_MERCURY
		dd	rm_buserr, rm_buserr, rm_buserr, rm_buserr, rm_buserr, rm_buserr, @Mcry_Read@4, rm_buserr
%else
		dd	rm_buserr, rm_buserr, rm_buserr, rm_buserr, rm_buserr, rm_buserr, rm_buserr, rm_buserr
%endif
		dd	@SRAM_Read@4, @SRAM_Read@4, @SRAM_Read@4, @SRAM_Read@4, @SRAM_Read@4, @SRAM_Read@4, @SRAM_Read@4, @SRAM_Read@4
		dd	rm_buserr, rm_buserr, rm_buserr, rm_buserr, rm_buserr, rm_buserr, rm_buserr, rm_buserr
		dd	rm_buserr, rm_buserr, rm_buserr, rm_buserr, rm_buserr, rm_buserr, rm_buserr, rm_buserr

		dd	rm_font, rm_font, rm_font, rm_font, rm_font, rm_font, rm_font, rm_font
		dd	rm_font, rm_font, rm_font, rm_font, rm_font, rm_font, rm_font, rm_font
		dd	rm_font, rm_font, rm_font, rm_font, rm_font, rm_font, rm_font, rm_font
		dd	rm_font, rm_font, rm_font, rm_font, rm_font, rm_font, rm_font, rm_font
		dd	rm_font, rm_font, rm_font, rm_font, rm_font, rm_font, rm_font, rm_font
		dd	rm_font, rm_font, rm_font, rm_font, rm_font, rm_font, rm_font, rm_font
		dd	rm_font, rm_font, rm_font, rm_font, rm_font, rm_font, rm_font, rm_font
		dd	rm_font, rm_font, rm_font, rm_font, rm_font, rm_font, rm_font, rm_font
		dd	rm_font, rm_font, rm_font, rm_font, rm_font, rm_font, rm_font, rm_font
		dd	rm_font, rm_font, rm_font, rm_font, rm_font, rm_font, rm_font, rm_font
		dd	rm_font, rm_font, rm_font, rm_font, rm_font, rm_font, rm_font, rm_font
		dd	rm_font, rm_font, rm_font, rm_font, rm_font, rm_font, rm_font, rm_font
		dd	rm_ipl, rm_ipl, rm_ipl, rm_ipl, rm_ipl, rm_ipl, rm_ipl, rm_ipl
		dd	rm_ipl, rm_ipl, rm_ipl, rm_ipl, rm_ipl, rm_ipl, rm_ipl, rm_ipl
		dd	rm_ipl, rm_ipl, rm_ipl, rm_ipl, rm_ipl, rm_ipl, rm_ipl, rm_ipl
		dd	rm_ipl, rm_ipl, rm_ipl, rm_ipl, rm_ipl, rm_ipl, rm_ipl, rm_ipl


MemWriteTable:
		dd	@TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8
		dd	@TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8
		dd	@TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8
		dd	@TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8
		dd	@TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8
		dd	@TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8
		dd	@TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8
		dd	@TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8, @TVRAM_Write@8
		dd	@CRTC_Write@8, wm_e82, @DMA_Write@8, wm_nop, @MFP_Write@8, @RTC_Write@8, wm_nop, @SysPort_Write@8
		dd	wm_opm, @ADPCM_Write@8, @FDC_Write@8, @SASI_Write@8, @SCC_Write@8, @PIA_Write@8, @IOC_Write@8, @WinDrv_Write@8;wm_nop
		dd	@SCSI_Write@8, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, @MIDI_Write@8
		dd	@BG_Write@8, @BG_Write@8, @BG_Write@8, @BG_Write@8, @BG_Write@8, @BG_Write@8, @BG_Write@8, @BG_Write@8
%ifndef	NO_MERCURY
		dd	wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, @Mcry_Write@8, wm_buserr
%else
		dd	wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr
%endif
		dd	@SRAM_Write@8, @SRAM_Write@8, @SRAM_Write@8, @SRAM_Write@8, @SRAM_Write@8, @SRAM_Write@8, @SRAM_Write@8, @SRAM_Write@8
		dd	wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr
		dd	wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr
; ROMエリアへの書きこみは全てバスエラー
		dd	wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr
		dd	wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr
		dd	wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr
		dd	wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr
		dd	wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr
		dd	wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr
		dd	wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr
		dd	wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr
		dd	wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr
		dd	wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr
		dd	wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr
		dd	wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr
		dd	wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr
		dd	wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr
		dd	wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr
		dd	wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr, wm_buserr


OPBaseTable:
		dd	setop_mem, setop_mem, setop_mem, setop_mem, setop_mem, setop_mem, setop_mem, setop_mem
		dd	setop_mem, setop_mem, setop_mem, setop_mem, setop_gvr, setop_gvr, setop_tvr, setop_rom


;-----------------------------------------------------------------------------
;  その他のでーた
;
_MEM:		dd	0
_IPL:		dd	0
_OP_ROM:	dd	0
_FONT:		dd	0
_BusErrFlag:	dd	0
_BusErrAdr:	dd	0
_AdrErrAdr:	dd	0
_MemByteAccess:	dd	0
RetValue:	dd	0
WriteValue:	dd	0
