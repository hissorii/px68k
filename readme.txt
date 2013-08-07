ポータブルX68000エミュレータ
PX68K ( Portable (x)keropi PRO-68K ) Ver.0.00
                                                                     2013/08/07

いろいろなデバイスでX68000エミュレータが動作したら楽しいな、
と思い立ち、このPX68Kを作りはじめました。

「Portable」とは「移植しやすいよ」という意味です。
タブレットやPSP等のポータブルデバイスで動いたらいいな、
という意味もこめています。


PX68Kは以下の成分からできています。

  けんじょさん (http://retropc.net/kenjo/) が
  作成された SHARP X68000エミュレータ WinX68k (通称けろぴー)

  NONAKA Kimihiro さん(http://www.asahi-net.or.jp/~aw9k-nnk/keropi/) が
  けろぴーをUNIX/X11 環境で動作する様に移植した xkeropi

  Stephane Dallongevilleさんが開発したものを、
  NJ氏がPSP用CPS1/CPS2/MVSP/NCDZに組み込んだMC68000 MPUエミュレータ


0. 注意事項

  0.1 全機種共通

  とりあえずGTK/X11用に作られていたメニューやらキー入力やらは
  全部取っ払ったので、今のところそれらはないです。
  現状はただ眺めるのみです。

  0.2 Android版について

  Androidは今回初めての開発につき、作法がまだよくわかりません。
  (Javaのコード一行も書いてないのにAndroidの開発と言えるのか?)
  特に/sdcard/ディレクトリってどのデバイスにもあるの?とか疑問が。

  一応デバッガ経由でのNexus 7、apkをインストールしてdocomo dtabで
  実機での動作確認をしていますが、他機種では動かないかもしれません。
  あと、解像度800x600未満の機種でも多分動きません。

1. 下準備

  1.1 UNIX系の場合

  ・xkeropiと同じ

  1.2 PSPの場合

  ・EBOOT.PBPと同じディレクトリに.keropiというディレクトリを作成し、
    BIOS ROM, フォントファイルを置く

  1.3 Androidの場合

   ・/sdcard/px68k/ ディレクトリを作成し、BIOS ROM, フォントファイルを置く

2. FDイメージの指定法

  2.1 UNIX系の場合

  ・引数で指定します。最初がFDD1, 次がFDD2。

    $ ./xkeropi hoge.xdf hogege.xdf

  2.2 PSPの場合

  ・FDD一枚目として読み込みたいイメージファイルをFDD1.XDFとして、
    EBOOT.PBPと同じディレクトリに置く。
    FDD二枚目の場合はFDD2.XDFとして置く。

  2.3 Androidの場合

  ・FDD一枚目として読み込みたいイメージファイルをFDD1.XDFとして、
    /sdcard/px68k/ ディレクトリに置く。
    FDD二枚目の場合はFDD2.XDFとして置く。

4. その他

  4.1 X68000のメインメモリ

  ・現在2MBにしています。

  4.1 メモリ1MBしか認識していないようですが?

  ・HUMAN68KのSWITCH.Xで指定するのですが、PSPでは現在キーボード入力が
    できません。他のエミュレーターで実行して、sram.datを持ってきてください。

  4.3 PSPで実行すると画面がはみ出たりするんですが?
  4.4 キー入力できないのですが?
  4.5 PSPで実行すると動作がものすごく遅いんですけど?
  4.6 音が出ないのですが?
  4.7 説明の番号が2から4に飛んでるんですけど?

      -> とりあえず現状はそんなものです。すみません。

5. 履歴

   2013/08/02 PSP版Ver0.00リリース
   2013/08/07 Android版Ver0.00リリース

5. 免責

  本ソフトウェアを使用したことによる、いかなる損害も作者は責任を負いません。
  あくまでも自己責任での使用をお願い致します。


ひっそりぃ(hissorii)/sakahi
http://hissorii.blog45.fc2.com (ひっそりぃドットコム)
http://emuhani.seesaa.net      (えみゅはに - Emulator Hacking 日記)
GitHub: https://github.com/hissorii/px68k
Twitter: @hissorii_com
