# ATtiny1606 / ATtiny806 DCC Decoder Firmware

NMRA規格の信号を受信して動作する、DCCデコーダーのファームウェアです。

[Microchip ATtiny1606](https://www.microchip.com/wwwproducts/en/ATtiny1606)で動作するように作成してあります。
<br>ファンクション機能のみに限定したバージョンは、[Microchip ATtiny806](https://www.microchip.com/wwwproducts/en/ATtiny806)でも動作するように作成してあります。
<br>(プログラム領域の都合上、ATtiny406などの下位モデルには搭載できません)

別のリポジトリに置いてある、対応する基板と組み合わせてご利用ください。

---

## ビルド環境

Microchip Studio (旧Atmel Studio)と、Arduino IDEでのビルドに対応しています。

  * Microchip Studioでビルドする場合： [source](source/)ディレクトリ内のMotorDecoder2.atsln
  * Arduino IDEでビルドする場合: [arduino](arduino/)ディレクトリ内から対応するスケッチに含まれる.inoファイルを開く

Arduinoを使用する場合は、[こちら](arduino/README.md)もお読みください。

---

## 主要スペック (フル機能版)
  * モーターのコントロールに対応
  * モーター制御は14-Step、28-Step、128-Stepに対応
  * スピードテーブル、最小電圧、中間電圧、最大電圧の設定に対応
  * モーターのBEMFの読み取り・フィードバック制御に対応 (CV60の設定で有効・無効を切り替え可能)
  * 7つのファンクション出力に対応 (通常状態)
  * モータードライバを、8番目のファンクション出力として動作するように設定可能 (CV43)
    <br>(モータードライバは前進後進自動切換え型のファンクションとして動作)
  * それぞれのファンクション出力に対し、F0-F28から任意の番号で動作し、かつ走行方向によって自動的に有効・無効が切り替わるように設定可能。
  * ヘッドライト減光機能を搭載し、停車時と走行時でヘッドライトに設定したファンクションの照度を自動的に変更する機能および、走行中でもファンクションの制御で減光状態に切り替え可能な機能を搭載。
  * 進行方向にかかわらず、ヘッドライトが常時点灯するように制御する機能を設定可能。
  * 特定のファンクション番号で、速度の上限を絞ることができる「入れ替えモード」を搭載。
  * 7-Bit Address、14-Bit Addressどちらの設定にも対応
  * Pagedモード、Directモード、Operationモードでのプログラミングに対応
  * アナログコントローラーでの操作に対応。
    <br>純直流出力コントローラーだけではなく、PWM出力方式のコントローラーにおいても問題なく制御可能。
    <br>(詳細は動作確認済みのコントローラー類を参照)
  * BiDi (Railcom) チャンネル1による車両アドレス情報の送出に対応

---

## 主要スペック (ファンクション限定版)

  * 7つのファンクション出力に対応 (通常状態)
  * 初期状態で、モータードライバを8番目のファンクション出力として動作するように設定 (CV43=1)
    <br>(モータードライバは前進後進自動切換え型のファンクションとして動作)
  * それぞれのファンクション出力に対し、F0-F28から任意の番号で動作し、かつ走行方向によって自動的に有効・無効が切り替わるように設定可能。
  * ヘッドライト減光機能を搭載し、停車時と走行時でヘッドライトに設定したファンクションの照度を自動的に変更する機能および、走行中でもファンクションの制御で減光状態に切り替え可能な機能を搭載。
  * 7-Bit Address、14-Bit Addressどちらの設定にも対応
  * Pagedモード、Directモード、Operationモードでのプログラミングに対応

---

## 動作確認を行っているマイコン

<table>
  <tr>
    <th>マイコン名</th>
    <th>動作確認を行ったバージョン</th>
  </tr>
  <tr>
    <td><a href="https://www.microchip.com/wwwproducts/en/ATtiny1606">ATtiny1606</a></td>
    <td>フル機能版</td>
  </tr>
  <tr>
    <td><a href="https://www.microchip.com/wwwproducts/en/ATtiny806">ATtiny806</a></td>
    <td>ファンクション限定版</td>
  </tr>
  <tr>
    <td><a href="https://www.microchip.com/wwwproducts/en/ATtiny816">ATtiny816</a></td>
    <td>ファンクション限定版
    <br>(ATtiny806用バイナリをリビルドなしで書き込み可)</td>
  </tr>
</table>

---

## 対応する基板設計データ

  * [ATtiny1606 DCC Decoder Board](https://github.com/ytsurui/dcc-decoder2-pcbdata/)

このデコーダ基板設計データには、下記の内容が含まれています。

  * [K3066RA](https://github.com/ytsurui/dcc-decoder2-pcbdata/blob/main/docs/readme-K3066.md)
  * [K3057RA](https://github.com/ytsurui/dcc-decoder2-pcbdata/blob/main/docs/readme-K3057.md)
  * [M02F7RA Roomlight-N](https://github.com/ytsurui/dcc-decoder2-pcbdata/blob/main/docs/readme-roomlight-n.md)

回路を独自に組んで動かす場合は、基板設計データに含まれる回路図を参考にしてください。

---

## 各種ドキュメント

  * [CV値一覧](docs/CVList.md)
  * [CV値一覧 (ファンクション限定版)](docs/CVList_tiny806.md)
  * [ファンクション番号の設定方法](docs/function.md)
  * [ATtiny1606のピン割り当て表](docs/attiny1606-pin.md)
  * [アナログ環境での運転・設定](docs/analogops.md)
  * [AVR Fuse Bitの設定内容](docs/avrfuse.md)

---

## 動作確認済みのコマンドステーション・コントローラー類
### DCC走行
  * Digitrax DCS50K (KATO D101)
  * Digitrax DCS100
  * Roco Z21
  * [DesktopStation DSair2](https://desktopstation.net/wiki/doku.php/dsair2)

### BiDi (Railcom) によるアドレス情報の受信
  * Railcom Train Addr Display
    <br>[Hardware: railcom-addrdisplay-pcb](https://github.com/ytsurui/railcom-addrdisplay-pcb) / [Software: railcom-addrdisplay-firmware](https://github.com/ytsurui/railcom-addrdisplay-firmware)
  * [DesktopStation BiDi(RailCom) Display](https://desktopstation.net/wiki/doku.php/railcomdisplay)
  * Lenz LRC120

### アナログ走行
  * TOMIX PowerUnit (5001)
  * TOMIX TCS PowerUnit N-1000CL (5502)
  * KATO パワーパックスタンダードS (22-012)
  * KATO パワーパックハイパーDX (22-017)
  * KATO パワーパックスタンダードSX (22-018)

---

## 現在判明している不具合
  * KATO パワーパックスタンダードSXで走行させようとした場合に、低速時(ボリューム8あたりより前)でギクシャク走行する
    <br>パワーパック側の低周波成分と、デコーダ側のモータースロー回転制御が競合しているためです。
    <br>パワーパック側のボリュームを8以上にすれば収まりますが、ソフト側での対策も考えています。
  * KATO D101に製品規定電圧以下の電源を接続した場合、モーター駆動がおかしくなり、速度制御が低速のままになる場合がある
    <br>現在調査中です。 (モーター種類によって発生する場合としない場合があるため、マイコン側にリセットが掛かっている可能性もあり？)

