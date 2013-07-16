
// DOSIO用のファイルタイプ

enum {
	FTYPE_NONE = 0,		// 自動判別 or PC
	FTYPE_SMIL,			// システム予約
	FTYPE_TEXT,			// テキストファイル
	FTYPE_BMP,			// Bitmap
	FTYPE_PICT,			// Picture (予約)
	FTYPE_PNG,			// Png (予約)
	FTYPE_WAV,			// Wave
	FTYPE_D88,			// D88
	FTYPE_BETA,			// ベタイメージ
	FTYPE_THD,			// .thd ハードディスクイメージ
	FTYPE_HDI,			// .hdi ハードディスクイメージ
	FTYPE_HDD,			// .hdd ハードディスクイメージ (予約)
	FTYPE_S98,			// .s98 ハードディスクイメージ
	FTYPE_MIMPI,			// mimpi defaultファイル
	FTYPE_SRAM			// SRAM
};
