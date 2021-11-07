# ATtiny1606 DCC Decoder CV List

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
    <td>ヘッドライト減光 (Default: F4)</td>
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
    <td>0</td>
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
    <td>47</td>
    <td>202</td>
    <td>キックスタート
    <br />Bit0-5: Timing, Bit6: Freq (0: 120Hz / 1: 60Hz), Bit7: Enable</td>
  </tr>
  <tr>
    <td>48</td>
    <td>7</td>
    <td>Asymmetric DCCの内部回路しきい値</td>
  </tr>
  <tr>
    <td>50</td>
    <td>255</td>
    <td>アナログ走行時のファンクション有効化フラグ
    <br>(bit8: Func8, bit7: Func7, ..., bit0: Func0)</td>
  </tr>
  <tr>
    <td>51</td>
    <td>5</td>
    <td>自動ブレーキ無効化モード (ファンクション)
    <br />0-28: F0-F28 / それ以外： 無効</td>
  </tr>
  <tr>
    <td>52</td>
    <td>0</td>
    <td>自動ブレーキ動作後に自動的に折り返し運転を開始するまでの時間
    <br />0: 無効 / 1-255: 指定秒数後に折り返し開始</td>
  </tr>
  <tr>
    <td>53</td>
    <td>10</td>
    <td>自動ブレーキ解除後の加速度</td>
  </tr>
  <tr>
    <td>54</td>
    <td>3</td>
    <td>自動ブレーキ動作時の減速度</td>
  </tr>
  <tr>
    <td>55</td>
    <td>30</td>
    <td>BEMFパラメーター (Kp)</td>
  </tr>
  <tr>
    <td>56</td>
    <td>10</td>
    <td>BEMFパラメーター (Ki)</td>
  </tr>
  <tr>
    <td>57</td>
    <td>40</td>
    <td>BEMFパラメーター (Kd)</td>
  </tr>
  <tr>
    <td>58</td>
    <td>127</td>
    <td>アナログ走行時の最大速度 (0-255)</td>
  </tr>
  <tr>
    <td>59</td>
    <td>3</td>
    <td>アナログ走行時の加減速度</td>
  </tr>
  <tr>
    <td>60</td>
    <td>131</td>
    <td>bit8: BEMFの有効化
    <br>bit1-bit0: モーター低速回転支援機能
    <br>(0: Disable / 1: 60Hz / 2: 120Hz / 3: 30Hz)</td>
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
  <tr>
    <td>131</td>
    <td>63</td>
    <td>入れ替えモード有効時の最高速度
    <br />(0-255)</td>
  </tr>
  <tr>
    <td>138</td>
    <td>100</td>
    <td>BEMF PID Feedback ADC Scaler MAX Value</td>
  </tr>
  <tr>
    <td>140</td>
    <td>0</td>
    <td>速度ゼロから動き出す際の通電開始までの起動時間
    <br>(1/10秒で指定、0.1秒から25.5秒まで / 値=0で無効)</td>
  </tr>

</table>