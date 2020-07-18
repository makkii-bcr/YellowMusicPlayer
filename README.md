# YellowMusicPlayer

Tonyu System 1（Tonyu1）用のBGM再生ソフトです。

MIDIファイル、Ogg Vorbisファイルが再生できます。

[プロジェクトボードのページ](https://www.tonyu.jp/project/viewProject.cgi?mainkey=667&)

## 動作環境

- Windows 10 / 8.1 / 8 / 7 / Vista / XP / Me / 2000 / 98SE

- DirectX 1 以降

## 仕様

|||
-|-
|プログラミング言語|C|
|API|Win32API|
||DirectSound|
||DirectMusic|

## 開発環境

|||
-|-
|コンパイラ|Borland C++ Compiler 5.5（BCC55）|
|IDE（ビルドに使う）|BCC Developer または Visual Windows for BC++|
|OS|Windows 10 / 7|

## ビルド方法 (ファイル等準備中)

Tonyu1の動作環境(Windows95以降・DirectX6以降)に合わせるため、古いBCC55でビルドしています。

（Visual Studio ではビルドできません）

Visual Windows for BC++ をインストールすると、BCC55もインストールされるので一番手っ取り早いです。

### Visual Windows for BC++でのビルド

http://phys.cool.coocan.jp/physjpn/bccwin.htm

1. 「BC++5.5付き」をダウンロードします。（bcw16-100S.zip）

1. 解凍してインストーラを開いて、インストールを進めます。

1. インストールしたら、Visual Windows for BC++を開きます。

    - ブラウザが開いたり、メッセージが色々出ますが、キャンセルでいいと思います。

1. 「ファイル(F)」→「プロジェクトを開く(R)」から「midiplayer2.vwp」を開きます。

1. DebugをReleaseにして、Releaseビルドにします。

1. 「ビルド(B)」→「リビルド(R)」でビルドします。

    - パスがあっていないと思うので、ビルドに失敗すると思います。

    - 「プロジェクト(P)」→「設定(E)」で、パス等を適切なものに置き換えてください。

    - ビルドが成功すると、Releaseフォルダに「Midiplayer2.exe」が作られます。

### アイコンの適用
BCC55では、影付きアイコンに対応していないため、影なしアイコンになります。

Resource Hackerでアイコンを再度設定すると、影付きアイコンになります。

### BCC Developerでのビルド

現在はBCC Developerでビルドしています。

同じくBCC55用のIDEです。

Visual Windows for BC++をインストールした後に、ダウンロードすると設定が楽です。

http://www.hi-ho.ne.jp/jun_miura/bccdev.htm

lzhファイルのため、解凍ソフトが必要です。

1. 起動後、「環境設定」→「コンパイラ」にBorland\Bcc55\Bin\bcc32.exeのパスを設定します。

1. 「midiplayer2_2.bdp」を開きます。

1. 「プロジェクト(P)」→「再構築(B)」でビルドします。

    - ビルドが成功すると、Releaseフォルダに「midiplayer2_2.exe」が作られます。

1. 「midiplayer2_2.exe」を「Midiplayer2.exe」にリネームします。
