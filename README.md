# DT_mesher
## 概要
このプログラムはデローニー三角形分割によるメッシュ分割を行うものである。
主に有限要素法のために使用することを考えている。
現在，Macでの動作確認しかしていないため，他の環境で使用した場合の動作を
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
'''bash
make
'''
を実行することでそれぞれのプログラムをコンパイルすることが出来る。

このプロジェクトのトップにもMakefileがある。
トップでmakeを実行するとmesher・meshvisのコンパイルを実行し，
その実行ファイルをbinディレクトリに格納するようになっている。
実際に使用する際はトップでのmakeを推奨する。

## 使用方法


## 入力ファイルの形式
