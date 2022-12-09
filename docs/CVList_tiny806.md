# ATtiny806 DCC Function Decoder CV List

<table>
  <tr>
    <td>CV番号</td>
    <td>初期値</td>
    <td>内容</td>
  </tr>
  <tr>
    <td>1</td>
    <td>3</td>
    <td>2桁アドレス (7-bit Address)</td>
  </tr>
  <tr>
    <td>2</td>
    <td>0</td>
    <td>始動電圧 (0-255)</td>
  </tr>
  <tr>
    <td>3</td>
    <td>0</td>
    <td>加速度 (0-31)</td>
  </tr>
  <tr>
    <td>4</td>
    <td>0</td>
    <td>減速度 (0-31)</td>
  </tr>
  <tr>
    <td>5</td>
    <td>255</td>
    <td>最大電圧 (0-255)</td>
  </tr>
  <tr>
    <td>6</td>
    <td>127</td>
    <td>中間電圧 (0-255)</td>
  </tr>
  <tr>
    <td>7</td>
    <td>1</td>
    <td>バージョン番号</td>
  </tr>
  <tr>
    <td>8</td>
    <td>13</td>
    <td>メーカーID</td>
  </tr>
  <tr>
    <td>15</td>
    <td>0</td>
    <td>デコーダロック番号</td>
  </tr>
  <tr>
    <td>16</td>
    <td>0</td>
    <td>デコーダロック番号</td>
  </tr>
  <tr>
    <td>17</td>
    <td>192</td>
    <td>4桁アドレス下位 (14-bit Address)</td>
  </tr>
  <tr>
    <td>18</td>
    <td>0</td>
    <td>4桁アドレス上位 (14-bit Address)</td>
  </tr>
  <tr>
    <td>19</td>
    <td>0</td>
    <td>Consist Address</td>
  </tr>
  <tr>
    <td>27</td>
    <td>3</td>
    <td>自動ブレーキの有効化
    <br />Bit1: 後進時 / Bit0: 前進時 (0: 無効 / 1: 有効)</td>
  </tr>
  <tr>
    <td>29</td>
    <td>14</td>
    <td>デコーダ基本設定</td>
  </tr>
  <tr>
    <td>33</td>
    <td>4</td>
    <td>Bit0-Bit6: ヘッドライト減光 ファンクション番号 (Default: F4)
    <br />Bit7 = 1: 手動減光モード</td>
  </tr>
  <tr>
    <td>34</td>
    <td>7</td>
    <td>入れ替え標識灯 (Default: F7)</td>
  </tr>
  <tr>
    <td>35</td>
    <td>64</td>
    <td>Func1 アドレス・ディレクション設定
    <br />(初期値: F0/前進)</td>
  </tr>
  <tr>
    <td>36</td>
    <td>129</td>
    <td>Func2 アドレス・ディレクション設定
    <br />(初期値: F1/後進)</td>
  </tr>
  <tr>
    <td>37</td>
    <td>129</td>
    <td>Func3 アドレス・ディレクション設定
    <br />(初期値: F1/後進)
  </tr>
  <tr>
    <td>38</td>
    <td>128</td>
    <td>Func4 アドレス・ディレクション設定
    <br />(初期値: F0/後進)
  </tr>
  <tr>
    <td>39</td>
    <td>65</td>
    <td>Func5 アドレス・ディレクション設定
    <br />(初期値: F1/前進)
  </tr>
  <tr>
    <td>40</td>
    <td>65</td>
    <td>Func6 アドレス・ディレクション設定
    <br />(初期値: F1/前進)
  </tr>
  <tr>
    <td>41</td>
    <td>3</td>
    <td>Func7 アドレス・ディレクション設定
    <br />(初期値: F3)
  </tr>
  <tr>
    <td>42</td>
    <td>0</td>
    <td>Func8 (Motor Driver) アドレス設定
    <br>(初期値: F0)</td>
  </tr>
  <tr>
    <td>43</td>
    <td>1</td>
    <td>Func8 Motor Driver 動作設定
    <br>0: 無効(モーター制御、初期値)
    <br>1: モータードライバ ファンクション動作</td>
  </tr>
  <tr>
    <td>44</td>
    <td>30</td>
    <td>ヘッドライト常時点灯モード (前位側)
    <br />(初期値: 無効)
  </tr>
  <tr>
    <td>45</td>
    <td>30</td>
    <td>ヘッドライト常時点灯モード (後位側)
    <br />(初期値: 無効)
  </tr>
  <tr>
    <td>50</td>
    <td>255</td>
    <td>アナログ走行時のファンクション有効化フラグ
    <br>(bit8: Func8, bit7: Func7, ..., bit0: Func0)</td>
  </tr>
  <tr>
    <td>61</td>
    <td>95</td>
    <td>モーター低速回転支援機能 始動時の最低デューティー</td>
  </tr>
  <tr>
    <td>62</td>
    <td>192</td>
    <td>モーター低速回転支援機能を停止する速度値</td>
  </tr>
  <tr>
    <td>63</td>
    <td>7</td>
    <td>入れ替え時用の最高速度抑制ファンクション
    <br />0-28: F0-F28 / 30: 無効</td>
  </tr>
  <tr>
    <td>64</td>
    <td>1</td>
    <td>CV29書き換え時のRailcom強制有効化 (いかなるCV29書き換えでもBit3を1にする)
    <br />0: 無効 / 1: 有効</td>
  </tr>
  <tr>
    <td>67-94</td>
    <td>-</td>
    <td>スピードテーブル (14step / 28step)</td> 
  </tr>
  
  <tr>
    <td>112</td>
    <td>64</td>
    <td>ヘッドライト減光機能作動時の照度
    <br />(7bit-4bit)</td>
  </tr>
  <tr>
    <td>114</td>
    <td>241</td>
    <td>Func1 照度およびファンクション種別
    <br>初期値: 照度15 (最大) / ヘッドライト</td>
  </tr>
  <tr>
    <td>115</td>
    <td>243</td>
    <td>Func2 照度およびファンクション種別
    <br>初期値: 照度15 (最大) / テールライト左</td>
  </tr>
  <tr>
    <td>116</td>
    <td>242</td>
    <td>Func3 照度およびファンクション種別
    <br>初期値: 照度15 (最大) / テールライト右</td>
  </tr>
  <tr>
    <td>117</td>
    <td>241</td>
    <td>Func4 照度およびファンクション種別
    <br>初期値: 照度15 (最大) / ヘッドライト</td>
  </tr>
  <tr>
    <td>118</td>
    <td>243</td>
    <td>Func5 照度およびファンクション種別
    <br>初期値: 照度15 (最大) / テールライト左</td>
  </tr>
  <tr>
    <td>116</td>
    <td>242</td>
    <td>Func6 照度およびファンクション種別
    <br>初期値: 照度15 (最大) / テールライト右</td>
  </tr>
  <tr>
    <td>120</td>
    <td>244</td>
    <td>Func7 照度およびファンクション種別
    <br>初期値: 照度15 (最大) / 室内灯タイプ1 (蛍光灯)</td>
  </tr>
  <tr>
    <td>121</td>
    <td>241</td>
    <td>Func8 (前進) 照度およびファンクション種別
    <br>初期値: 照度15 (最大) / ヘッドライト</td>
  </tr>
  <tr>
    <td>122</td>
    <td>242</td>
    <td>Func8 (後進) 照度およびファンクション種別
    <br>初期値: 照度15 (最大) / テールライト右</td>
  </tr>

</table>