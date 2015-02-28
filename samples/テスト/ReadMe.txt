YellowMusicPlayerのテストプログラムです。（YellowMusicPlayerのデバッグのために使っていたものです）
YellowMusicPlayerでどんなことができるか大体わかると思います。

画面を見ればだいたい操作方法がわかると思います。

Enterでリストプレイヤーモードに切り替えることができ、
登録されたMidi,Oggを順に再生するようになります。

ShiftキーでMidiとOggの操作の切り替えができます。

エフェクトや再生オプションは、カーソルキーで操作できます。
　右の項目の上３つは、再生中に変更できる項目
　真ん中の４つ(Oggでは６つ)は、再生前に変更できる項目（フェードアウトは停止前に変更できる）
　下の５つは、再生前に変更できる項目(Midiのみ)
  (マスターピッチが上手くいかないMidiもあえて付属しています)

MusicPlayerの状態取得は、0で無効、1～1000で有効になります。
値が小さいほど、更新頻度が高くなります。
1000は定期的な書き込みは行われず、再生・停止・一時停止・一時停止から再開の時のみ更新されます。

O(オー)キーを押すとオプション画面になり、DirectMusicのON/OFFの切り替えなどができます。
Enterを押すと設定が適応され、O(オー)キーを押すと設定をキャンセルします。
DirectMusicのON/OFFを切り替えた時は、Midiは停止し音源が変わります。

再生したいMidi,Oggを簡単に登録できます。

Midiは、Tonyu開発環境版でMidiを登録するだけでＯＫです。
Usrフォルダに再生したいMidiを置いてください。
Tonyu開発環境版でプロジェクトを開き、「ウィンドウ」→「BGM・効果音」→「自動追加」ボタンを押し、
「実行」→「実行」→　Tonyuを閉じてください。
あとは、ランタイム版を開けば登録されているので、選曲して再生することができます。

Oggは、filesフォルダにある「oggFileList.txt」にファイル名（拡張子も含む）を１行ずつ書いてください。
Usrフォルダに再生したいOggを置くと再生されます。


付属の Midi, Ogg は使わせて頂いただいたものがほとんどです。
使わせて頂きありがとうございます。

「hy_joe.mid」                  H.YAMANOIさん
「kuma.mid」                    モモトロさん
「Once_Upon_A_Time.mid」        Cafe Au Laitさん  http://www.cyberbb.com/cafe_au_lait/
「Para01.mid」                  zxcさん
「syuBoss.mid」「syuFast.mid」  自作

「BGM022.ogg」「BGM036.ogg」「BGM094.ogg」
        Pianos DauGeさん  http://pianosdauge.org/

「tamco03l.mid」「tam-n11l.mid」「tam-n13l.mid」
「Fairwind_loop.ogg」「tam-n11_loop.ogg」「tam-n13loop.ogg」
        TAM Music Factoryさん  http://www.tam-music.com/



付属の TAM Music Factoryさんの Midi, Ogg は、全てRPGツクール用のループに対応しています。
YellowMusicPlayerでも、RPGツクール用のループが使えます。
OggでRPGツクール用ループをする場合は、「Oggループ使用:」を1にしてから再生してください。