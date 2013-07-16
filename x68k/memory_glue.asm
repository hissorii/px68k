        BITS 32

        SECTION .data

%define _MEM            MEM
%define _IPL            IPL
%define _OP_ROM         OP_ROM
%define _BusErrFlag     BusErrFlag
%define _BusErrAdr      BusErrAdr
%define _AdrErrAdr      AdrErrAdr
%define _MemByteAccess  MemByteAccess


	extern	VCtrl_Read
	extern	CRTC_Read
	extern	GVRAM_Read
	extern	TVRAM_Read
	extern	BG_Read
	extern	Pal_Read
	extern	DMA_Read
	extern	MFP_Read
	extern	RTC_Read
	extern	OPM_Read
	extern	ADPCM_Read
	extern	MIDI_Read
	extern	FDC_Read
	extern	SASI_Read
	extern	SCC_Read
	extern	PIA_Read
	extern	IOC_Read
	extern	SRAM_Read
	extern	SysPort_Read
	extern	WinDrv_Read
	extern	Mcry_Read
	extern	SCSI_Read

	extern	VCtrl_Write
	extern	CRTC_Write
	extern	GVRAM_Write
	extern	TVRAM_Write
	extern	BG_Write
	extern	Pal_Write
	extern	DMA_Write
	extern	MFP_Write
	extern	RTC_Write
	extern	OPM_Write
	extern	ADPCM_Write
	extern	MIDI_Write
	extern	FDC_Write
	extern	SASI_Write
	extern	SCC_Write
	extern	PIA_Write
	extern	IOC_Write
	extern	SRAM_Write
	extern	SysPort_Write
	extern	WinDrv_Write
	extern	Mcry_Write
	extern	SCSI_Write

	extern	Memory_ErrTrace

        SECTION .text

	global	@VCtrl_Read@4
	global	@CRTC_Read@4
	global	@GVRAM_Read@4
	global	@TVRAM_Read@4
	global	@BG_Read@4
	global	@Pal_Read@4
	global	@DMA_Read@4
	global	@MFP_Read@4
	global	@RTC_Read@4
	global	@OPM_Read@4
	global	@ADPCM_Read@4
	global	@MIDI_Read@4
	global	@FDC_Read@4
	global	@SASI_Read@4
	global	@SCC_Read@4
	global	@PIA_Read@4
	global	@IOC_Read@4
	global	@SRAM_Read@4
	global	@SysPort_Read@4
	global	@WinDrv_Read@4
	global	@Mcry_Read@4
	global	@SCSI_Read@4

	global	@VCtrl_Write@8
	global	@CRTC_Write@8
	global	@GVRAM_Write@8
	global	@TVRAM_Write@8
	global	@BG_Write@8
	global	@Pal_Write@8
	global	@DMA_Write@8
	global	@MFP_Write@8
	global	@RTC_Write@8
	global	@OPM_Write@8
	global	@ADPCM_Write@8
	global	@MIDI_Write@8
	global	@FDC_Write@8
	global	@SASI_Write@8
	global	@SCC_Write@8
	global	@PIA_Write@8
	global	@IOC_Write@8
	global	@SRAM_Write@8
	global	@SysPort_Write@8
	global	@WinDrv_Write@8
	global	@Mcry_Write@8
	global	@SCSI_Write@8

	global	@Memory_ErrTrace@0

;
; read
;
	ALIGN	4
@VCtrl_Read@4:
	pushad
	push	ecx
	call	VCtrl_Read
	pop	ecx
	mov	[esp + 28], al
	popad
	ret

	ALIGN	4
@CRTC_Read@4:
	pushad
	push	ecx
	call	CRTC_Read
	pop	ecx
	mov	[esp + 28], al
	popad
	ret

	ALIGN	4
@GVRAM_Read@4:
	pushad
	push	ecx
	call	GVRAM_Read
	pop	ecx
	mov	[esp + 28], al
	popad
	ret

	ALIGN	4
@TVRAM_Read@4:
	pushad
	push	ecx
	call	TVRAM_Read
	pop	ecx
	mov	[esp + 28], al
	popad
	ret

	ALIGN	4
@BG_Read@4:
	pushad
	push	ecx
	call	BG_Read
	pop	ecx
	mov	[esp + 28], al
	popad
	ret

	ALIGN	4
@Pal_Read@4:
	pushad
	push	ecx
	call	Pal_Read
	pop	ecx
	mov	[esp + 28], al
	popad
	ret

	ALIGN	4
@DMA_Read@4:
	pushad
	push	ecx
	call	DMA_Read
	pop	ecx
	mov	[esp + 28], al
	popad
	ret

	ALIGN	4
@MFP_Read@4:
	pushad
	push	ecx
	call	MFP_Read
	pop	ecx
	mov	[esp + 28], al
	popad
	ret

	ALIGN	4
@RTC_Read@4:
	pushad
	push	ecx
	call	RTC_Read
	pop	ecx
	mov	[esp + 28], al
	popad
	ret

	ALIGN	4
@OPM_Read@4:
	pushad
	push	ecx
	call	OPM_Read
	pop	ecx
	mov	[esp + 28], al
	popad
	ret

	ALIGN	4
@ADPCM_Read@4:
	pushad
	push	ecx
	call	ADPCM_Read
	pop	ecx
	mov	[esp + 28], al
	popad
	ret

	ALIGN	4
@MIDI_Read@4:
	pushad
	push	ecx
	call	MIDI_Read
	pop	ecx
	mov	[esp + 28], al
	popad
	ret

	ALIGN	4
@FDC_Read@4:
	pushad
	push	ecx
	call	FDC_Read
	pop	ecx
	mov	[esp + 28], al
	popad
	ret

	ALIGN	4
@SASI_Read@4:
	pushad
	push	ecx
	call	SASI_Read
	pop	ecx
	mov	[esp + 28], al
	popad
	ret

	ALIGN	4
@SCC_Read@4:
	pushad
	push	ecx
	call	SCC_Read
	pop	ecx
	mov	[esp + 28], al
	popad
	ret

	ALIGN	4
@PIA_Read@4:
	pushad
	push	ecx
	call	PIA_Read
	pop	ecx
	mov	[esp + 28], al
	popad
	ret

	ALIGN	4
@IOC_Read@4:
	pushad
	push	ecx
	call	IOC_Read
	pop	ecx
	mov	[esp + 28], al
	popad
	ret

	ALIGN	4
@SRAM_Read@4:
	pushad
	push	ecx
	call	SRAM_Read
	pop	ecx
	mov	[esp + 28], al
	popad
	ret

	ALIGN	4
@SysPort_Read@4:
	pushad
	push	ecx
	call	SysPort_Read
	pop	ecx
	mov	[esp + 28], al
	popad
	ret

	ALIGN	4
@WinDrv_Read@4:
%if 1
	mov	al, 0ffh
%else
	pushad
	push	ecx
	call	WinDrv_Read
	pop	ecx
	mov	[esp + 28], al
	popad
%endif
	ret

	ALIGN	4
@Mcry_Read@4:
	pushad
	push	ecx
	call	Mcry_Read
	pop	ecx
	mov	[esp + 28], al
	popad
	ret

	ALIGN	4
@SCSI_Read@4:
	pushad
	push	ecx
	call	SCSI_Read
	pop	ecx
	mov	[esp + 28], al
	popad
	ret


;
; write
;
	ALIGN	4
@VCtrl_Write@8:
	pushad
	push	edx
	push	ecx
	call	VCtrl_Write
	pop	ecx
	pop	edx
	popad
	ret

	ALIGN	4
@CRTC_Write@8:
	pushad
	push	edx
	push	ecx
	call	CRTC_Write
	pop	ecx
	pop	edx
	popad
	ret

	ALIGN	4
@GVRAM_Write@8:
	pushad
	push	edx
	push	ecx
	call	GVRAM_Write
	pop	ecx
	pop	edx
	popad
	ret

	ALIGN	4
@TVRAM_Write@8:
	pushad
	push	edx
	push	ecx
	call	TVRAM_Write
	pop	ecx
	pop	edx
	popad
	ret

	ALIGN	4
@BG_Write@8:
	pushad
	push	edx
	push	ecx
	call	BG_Write
	pop	ecx
	pop	edx
	popad
	ret

	ALIGN	4
@Pal_Write@8:
	pushad
	push	edx
	push	ecx
	call	Pal_Write
	pop	ecx
	pop	edx
	popad
	ret

	ALIGN	4
@DMA_Write@8:
	pushad
	push	edx
	push	ecx
	call	DMA_Write
	pop	ecx
	pop	edx
	popad
	ret

	ALIGN	4
@MFP_Write@8:
	pushad
	push	edx
	push	ecx
	call	MFP_Write
	pop	ecx
	pop	edx
	popad
	ret

	ALIGN	4
@RTC_Write@8:
	pushad
	push	edx
	push	ecx
	call	RTC_Write
	pop	ecx
	pop	edx
	popad
	ret

	ALIGN	4
@OPM_Write@8:
	pushad
	push	edx
	push	ecx
	call	OPM_Write
	pop	ecx
	pop	edx
	popad
	ret

	ALIGN	4
@ADPCM_Write@8:
	pushad
	push	edx
	push	ecx
	call	ADPCM_Write
	pop	ecx
	pop	edx
	popad
	ret

	ALIGN	4
@MIDI_Write@8:
	pushad
	push	edx
	push	ecx
	call	MIDI_Write
	pop	ecx
	pop	edx
	popad
	ret

	ALIGN	4
@FDC_Write@8:
	pushad
	push	edx
	push	ecx
	call	FDC_Write
	pop	ecx
	pop	edx
	popad
	ret

	ALIGN	4
@SASI_Write@8:
	pushad
	push	edx
	push	ecx
	call	SASI_Write
	pop	ecx
	pop	edx
	popad
	ret

	ALIGN	4
@SCC_Write@8:
	pushad
	push	edx
	push	ecx
	call	SCC_Write
	pop	ecx
	pop	edx
	popad
	ret

	ALIGN	4
@PIA_Write@8:
	pushad
	push	edx
	push	ecx
	call	PIA_Write
	pop	ecx
	pop	edx
	popad
	ret

	ALIGN	4
@IOC_Write@8:
	pushad
	push	edx
	push	ecx
	call	IOC_Write
	pop	ecx
	pop	edx
	popad
	ret

	ALIGN	4
@SRAM_Write@8:
	pushad
	push	edx
	push	ecx
	call	SRAM_Write
	pop	ecx
	pop	edx
	popad
	ret

	ALIGN	4
@SysPort_Write@8:
	pushad
	push	edx
	push	ecx
	call	SysPort_Write
	pop	ecx
	pop	edx
	popad
	ret

	ALIGN	4
@WinDrv_Write@8:
%if 0
	pushad
	push	edx
	push	ecx
	call	WinDrv_Write
	pop	ecx
	pop	edx
	popad
%endif
	ret

	ALIGN	4
@Mcry_Write@8:
	pushad
	push	edx
	push	ecx
	call	Mcry_Write
	pop	ecx
	pop	edx
	popad
	ret

	ALIGN	4
@SCSI_Write@8:
	pushad
	push	edx
	push	ecx
	call	SCSI_Write
	pop	ecx
	pop	edx
	popad
	ret


;
; memory trace
;
@Memory_ErrTrace@0:
%if 0
	; XXX
%endif
	ret
