# simple model
ハードウェア version 1 の検証用に、おおよそ一致している軸配置とリンク構成を持つモデル。<br>
<br>

モータは [Steadywin 8108-8](https://steadywin.cn/pd.jsp?fromColId=0&id=134#_pp=0_629_3) を想定。トルクなどジョイントの制限はそのモータを参考。<br>
GIM8108-8 (抜粋)<br>
 - torque (nominal): 7.5 [Nm]
 - torque (stall)  : 22 [Nm]
 - nominal speed after reduce: 110 [rpm] -> 11.519173063 [rad/s]
 - max speed after reduce    : 320 [rpm] -> 33.510321638 [rad/s]
 - weight (with driver): 396 [g] = 0.396 [kg]
<br>

質量は以下のように決定<br>
 - mass [kg] = 1\[kg](link-mass) + 0.396\[kg](motor-mass) * 2\(motor-quantity (0 ~ 2))
<br>

慣性は、以下のサイトを参考に導出<br>
 - https://en.wikipedia.org/wiki/List_of_moments_of_inertia#List_of_3D_inertia_tensors
  - ただし、wr2wp_link はL字であるため、三平方の定理から始端～末端までの直線距離と等しい長さの棒で近似
<br>

リンク長さは以下のように決定<br>
 - wr(waist-roll)(right) to wr(left): 0.21 [m]
 - wr to wp: 0.085 [m] & 90[deg] & 0.0465 [m]
 - wp to kp: 0.18 [m]
 - kp to heel: 0.184 [m]
<br>

~~注意事項として、ロボットの原点（base_link）は body の中心にあるため、world の原点に合わせて spawn させると、脚が埋まった状態から始まる。問題になる場合は、脚の長さ分、base_link（だけ？）の origin を、z 方向に＋する必要があるだろう。~~（解決済み） <br>

![Gho4AJFbYAAhDS4](https://github.com/user-attachments/assets/b6833fec-134b-431c-afbd-884bc0461e09)
