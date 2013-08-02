ポータブルX68000エミュレータ
PX68K ( Portable (x)keropi PRO-68K ) Ver.0.00
                                                                     2013/08/02

けんじょさん (http://retropc.net/kenjo/) が
作成された SHARP X68000エミュレータ WinX68k (通称けろぴー)を、

NONAKA Kimihiro さん(http://www.asahi-net.or.jp/~aw9k-nnk/keropi/) が
UNIX/X11 環境で動作する様に移植した xkeropi を

公開されています。

その xkeropi をベースにして、より移植性(portability)を高めるべく
開発中なのがPX68Kなのです。

とりあえずGTK/X11用に作られていたメニューやらキー入力やらは
全部取っ払ったので、今のところそれらはないです。

0. 下準備

  0.1 UNIX系の場合

  ・xkeropiと同じ。

  0.2 PSPの場合

  ・EBOOT.PBPと同じディレクトリに.keropiというディレクトリを作成し、
    BIOS ROM, フォントファイルを置く。

1. FDイメージの指定法

  1.1 UNIX系の場合

  ・引数で指定します。最初がFDD1, 次がFDD2。

    $ ./xkeropi hoge.xdf hogege.xdf

  1.2 PSPの場合

  ・FDD一枚目として読み込みたいイメージファイルをFDD1.XDFとして、
    EBOOT.PBPと同じディレクトリに置く。
    FDD二枚目の場合はFDD2.XDFとして置く。

3. その他

  3.1 X68000のメインメモリ

  ・現在2MBにしています。

  3.1 メモリ1MBしか認識していないようですが?

  ・HUMAN68KのSWITCH.Xで指定するのですが、PSPでは現在キーボード入力が
    できません。他のエミュレーターで実行して、sram.datを持ってくれば
    うまくいくかも。

  3.3 PSPで実行すると画面がはみ出たりするんですが?
  3.4 PSPで実行するとキー入力できないのですが?
  3.5 PSPで実行すると動作がものすごく遅いんですけど?
  3.6 音が出ないのですが?
  3.7 説明の番号が1から3に飛んでるんですけど?

      -> とりあえず現状はそんなものです。すみません。


ひっそりぃ(hissorii)/sakahi
http://hissorii.blog45.fc2.com ひっそりぃドットコム
http://emuhani.seesaa.net      えみゅはに - Emulator Hacking 日記
GitHub: https://github.com/hissorii/px68k
Twitter: @hissorii_com
