
// X68KのCGROMを作る
// buf:           作る領域 (0xc0000バイト分のバッファ)
// x68030:        X68030フォントを作るか？ (現在無効)
// primaryface:   書体１ (主に16ドット用)
// secondaryface: 書体２ (主に24ドット用)
//
// 戻り値:         FALSE: 失敗, TRUE: 成功


int make_cgromdat(BYTE *buf, int x68030, LPSTR primaryface, LPSTR secondaryface);
