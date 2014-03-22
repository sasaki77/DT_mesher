# DT_mesher
## 概要
このプログラムはデローニー三角形分割によるメッシュ分割を行うものである。
主に有限要素法のために使用することを考えている。
現在，デローニー三角形分割を利用して，次の4つの要素で分割することが可能である。

* 三角形1次要素 
* 三角形2次要素
* 四角形1次要素 
* 四角形2次要素 

今のところ，Macでの動作確認しかしていないため，他の環境で使用した場合の動作を
保証することはできない。

作成に当たって，以下のサイトも参考にしている。

[メッシュ生成のプログラミングTIPS](http://mf-atelier.sakura.ne.jp/mf-atelier/modules/tips/index.php/program/algorithm/a1.html)

[月の杜工房 - 2直線の交点を求める](http://ums.futene.net/wiki/MESH/A5E1A5C3A5B7A5E5C0B8C0AEA4CEA5D7A5EDA5B0A5E9A5DFA5F3A5B054495053.html)

## ファイル構成
このプログラムは以下の2種類のプログラムを含んでいる。

* mesh：入力ファイルの領域をデローニー三角形分割するプログラム
* meshvis：meshの結果をOpenGLで表示するプログラム

上の2つのプログラムはそれぞれ，mesher・meshvisのディレクトリに入っている。

mesherのプログラムはmeshvisを使用して結果表示を行うようになっているため，
mesherとmeshvisはbinディレクトリに入れて使用することを推奨する。

その他，docディレクトリにはプログラムのクラス構成図が含まれている。

## コンパイル
mesherとmeshvisディレクトリ内で
```bash
make
```
を実行することでそれぞれのプログラムをコンパイルすることが出来る。

このプロジェクトのトップにもMakefileがある。

トップでmakeを実行するとmesher・meshvisのコンパイルを実行し，
その実行ファイルをbinディレクトリに格納するようになっている。

実際に使用する際はトップでのmakeを推奨する。

## 使用方法
### mesh（メッシュ分割）
あらかじめエディタなどで拡張子が".in"の入力ファイルを作成しておく。

ここでは"sample.in"を例に取る。

実行ファイル"mesh"を次のように実行する。

```bash
./mesh sample
```

引数として渡すのは，入力ファイルの".in"を取り除いたものである。

実行した時に，分割する要素を三角形1次要素・三角形2次要素としている場合
次のように表示される。

```bash
number of additional nodes : 
 >>
```

この後に，どれだけの節点を追加するのかを入力する。

追加する節点を入力し，分割に成功すると次のようにmeshvisを呼び出して
画面表示を行うかを問われる。

```bash
launch visualizer ? (y/n) 
 >>
```
yを入力するとmeshvisを呼び出して画面表示を行い，nを入力するとそのまま
meshを終了する。

分割に成功すると次の3つのファイルが入力ファイルと同じディレクトリに作成される。

* "*.bc"：境界条件の情報ファイル
* "*.elem":節点と要素の情報ファイル
* "*.bs"：要素分割の結果表示のために使用される情報

### meshvis（メッシュの表示）
メッシュの描画には次の2つのファイルが必要になる。
* "*.elem":節点と要素の情報ファイル
* "*.bs"：要素分割の結果表示のために使用される情報

ここでも"sample.elem"，"sample.bs"を例にとって説明する。
メッシュの描画は次のように実行する。

```bash
./meshvis sample
```

操作方法は次の通りである
* q:画面を閉じる
* s:画面を eps ファイルで保存する
* b:境界描画の ON・OFF
* n:節点番号描画の ON・OFF

## 入力ファイルの形式
入力ファイルには次の情報が必要になる。

* 使用する要素形状
* 境界を構成する節点の情報
* 境界の辺の情報
* 境界の情報

以下に1/4の円の入力情報の例を示す。

なお，このファイルは"bin/data/sample"のディレクトリ以下に置いたので，
binディレクトリにmesh実行ファイルを置けば次のように実行できる。

```bash
./mesh data/sample/sample
```

```bash
# メッシュ生成プログラムの新規作成
# ================================
#
# 1.達成すべき機能
#  　凹凸・複数領域の同時分割
# 　円による曲線の表現
# 　節点の追加方法
# 　　1. 偏平率分割
# 　　2. 密度分割
# 　　3. 格子分割
# 　メインプロセスとの連携
# 　　1. 一次三角形要素
# 　　2. 二次三角形要素
# 　　3. 一次四角形要素
# 　　4. 二次四角形要素
#
# 2.入力
# 入力ファイルの形式を以下に示す．

# ========== 1/4 円問題 ==========

$begin_params
# パラメータを記述する領域

# $form は以下の通りの要素形状と次数を指定する．
# 引数 対応する要素形状と次数
# 1    tri-1st order
# 2    tri-2nd order
# 3    quad-1st order
# 4    quad-2nd order
# $form のoption
# -obl 偏平率分割をするoption
#      (四角形分割の際に格子点分割でなく偏平率分割をしたい場合に使用)
$form 2

# $intervalは四角形要素分割の際の格子点間隔を指定
$interval 0.25

$end

$begin_node
# ノードの設定　番号，座標を記述する．
# number  x    y
  1       0    0  
  2       0    1 
  3       1    0 
$end

$begin_edge
# 辺の設定
# strの引数は 分割数
# cirの引数は 分割数,中心x,中心y,回転角度(右回り)
# 境界条件(bc)はノイマン条件　ディリクレ条件を指定　引数はその辺上の既知の値
# 今のところ分割数は$noneと指定すること．(2011/09/08)
# number node1 node2 line  line-args     bc         bc-args material
    1      1     2     $str  $none         $neumann   $none   $none
    2      2     3     $cir  $none,0,0,90  $neumann   $none   $Cu
    3      3     1     $str  $none         $dirichlet 0       $none
$end

$begin_boundary
# 境界の設定 境界外周のノードを順序通り格納
# isOuterは$trueで外部境界 $falseで内部境界
# inclusionは境界自身の番号と境界が包含する境界の番号を入れること．
# number nodes isOuter inclusion 
  1      1,2,3 $true   1
$end
