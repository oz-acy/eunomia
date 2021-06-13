# LIBEUNOMIA

## これはなに?
作者がこれまで構築したコードの中から
環境非依存で再利用可能な部分をライブラリとして纏めておかうと企圖したもの。


## Eunomiaとは?
[女神エウノミアー](https://ja.wikipedia.org/wiki/%E3%82%A8%E3%82%A6%E3%83%8E%E3%83%9F%E3%82%A2%E3%83%BC)の名を拜借した。


## 内容
ライブラリの提供する機能を、ヘッダファイル別に示す。

|ヘッダファイル|内容|
|:---|:---|
|eunomia/exception.h|例外|
|eunomia/noncopyable.h|CRTPによるコピー禁止用クラステンプレート|
|eunomia/debuglogger.h|デバッグ用ロガー|
|eunomia/imagebuffer.h|畫素表現型をパラメタとする畫像バッファクラステンプレート|
|eunomia/colour.h|RGB24bit色情報クラス及びRGBA32bit色情報クラス|
|eunomia/hexpainter.h|ヘクスマップ向け正六角形描畫用クラステンプレート|
|eunomia/picture.h|RGB24bitの畫像バッファクラス|
|eunomia/picture_indexed.h|RGB24bit256インデックスの畫像バッファクラス|
|eunomia/picture_rgba.h|RGBA32bitの畫像バッファクラス|
|eunomia/pngio.h|PNGファイルの入出力|
|eunomia/jpegio.h|JPEGファイルの入出力|
|eunomia/dibio.h|DIBファイルの入出力|
|eunomia/utility.h|ユーティリティー|

ライブラリの外、ライブラリを用ゐた畫像擴大縮小用のコンソールアプリケーションresizerと、ヘクスマップ描畫用クラステンプレートの利用サンプルhextestを提供する。

### resizer
畫像の擴大や縮小を行ふコンソールアプリケーション。
次のやうに對象ファイルやオプションを指定して使用する。

```bash
$ resizer -p 倍率 元畫像のpath 結果畫像のpath
$ resizer -l 處理後の畫像の幅 處理後の畫像の高さ 元畫像のpath 結果畫像のpath
```

畫像フォーマットは擴張子で判別する。入力フォーマットと出力フォーマットが異なつてゐても問題ない。
對應する擴張子は、.bmp、.png、.jpg、.jpeg。


### hextest
實行すると現在のディレクトリにhextest.pngといふ畫像ファイルを生成する。


## 依存してゐるライブラリ
PNGの入出力は以下に依存してゐる。
* [libpng](http://www.libpng.org/pub/png/libpng.html)
* [zlib](https://www.zlib.net/)

JPEGの入出力は以下に依存してゐる。
* [libjpeg](http://www.ijg.org/)

各ライブラリの作者に謝意と敬意を表する。


## 利用方法

### 導入
CMakeを用ゐてインストールする。

```bash
$ cmake -G "Unix Makefiles" .
$ make
$ make install
```

ジェネレータは環境に應じて適宜指定すべし。場合によつてはCMakeにあれこれオプションを指定する必要があるかもしれない。

ヘッダファイルは${prefix}/include/eunomiaの下にインストールされる。

ライブラリを利用するときには、CMakeを用ゐるのであれば find_package(eunomia) すれば色々捗る、はず。


### リファレンス
Doxygenを用ゐて生成したリファレンスを
<https://www.hiemalis.org/~acy/swlib/eunomia/>
に置いてゐる。


## ライセンス
[二條項BSDライセンス](https://opensource.org/licenses/BSD-2-Clause)とする。


## 作者
oZ/acy (名賀月晃嗣)
* <acy@hiemalis.org>
* <https://www.hiemalis.org/>
