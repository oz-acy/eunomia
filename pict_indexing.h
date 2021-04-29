/*
 * Copyright 2003-2021 oZ/acy (名賀月晃嗣)
 * Redistribution and use in source and binary forms, 
 *     with or without modification, 
 *   are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/*
 * @author oZ/acy (名賀月晃嗣)
 * @brief 畫像のインデックス化に用ゐる函數の實裝
 *
 * @date 2021.4.25 LIBPOLYMNIAからLIBEUNOMIAへの移植に伴ひヘッダファイルに摘出
 */
#ifndef INCLUDE_GUARD_EUNOMIA_PICTURE_INDEXING_IMPLEMENT_H
#define INCLUDE_GUARD_EUNOMIA_PICTURE_INDEXING_IMPLEMENT_H

#include <algorithm>
#include <memory>
#include <vector>
#include <climits>
#include "picture_indexed.h"


namespace eunomia::implement_
{
//// メディアンカットによるパレット生成 ////////
/**
 * @brief 色要素の範圍情報
 */
class ColourChannelRange_
{
public:
  std::uint8_t min;
  std::uint8_t max;

  /// @brief 色要素値の分布を表すビットフィールド
  // x が [min, max] に存在するなら、bits[x >> 5] の x & 0x1f bit が 1
  std::uint32_t bits[8];

  std::uint32_t sum; ///< 範圍内の色要素値の總計

public:
  ColourChannelRange_() noexcept
    : min(0), max(255), bits{0, 0, 0, 0, 0, 0, 0, 0}, sum(0)
    {}

  /// @brief 範圍内か否かの確認
  bool isWithin(std::uint8_t v) const noexcept
    { return v >= min && v <= max; }

  /// @brief 分布情報の削除
  void clear()
  {
    std::fill_n(bits, 8, 0);
    sum = 0;
  }

  /// @brief 色要素値の加算
  void add(std::uint8_t v) noexcept
  {
    sum += v;
    bits[v >> 5] |= (std::uint32_t)1 << (v & 0x1f);
  }

  /// @brief 範圍情報の切り詰め
  ///
  /// 分布情報bitsに從つてminを切り上げmaxを切り下げる。
  int cutoff()
  {
    // bits[]の内容に從つて最小値minを切り上げる
    for (int j = min >> 5; j < 8; j++)
      if (bits[j]) {
        int i, f;
        // 1であるビットまで讀み飛ばす
        for (i = 0, f = 1; i < 32 && !(bits[j] & f); ++i, f <<= 1)
          ;
        
        i += j << 5;
        if (i <= max)
          min = i;

        break;
      }

    // bits[]の内容に從つて最大値maxを切り下げる
    for (int j = max >> 5; j >= 0; j--)
      if (bits[j]) {
        int i, f;
        // 1であるビットまで讀み飛ばす
        for (i = 31, f = 1 << 31; i >= 0 && !(bits[j] & f); --i, f >>= 1)
          ;

        i += j << 5;
        if (i >= min)
          max = i;

        break;
      }

    // 區間の幅を返す
    return max - min + 1;
  }
};


/**
 * @brief メディアンカットで用ゐるヒストグラムの範圍情報
 */
class PixelRange_
{
public:
  unsigned count;  ///< 範圍内の畫素數
  ColourChannelRange_ red; ///< 赤要素の範圍
  ColourChannelRange_ green; ///< 緑要素の範圍
  ColourChannelRange_ blue; ///< 青要素の範圍

public:
  PixelRange_() noexcept : count(0) {}

  /// @brief 範圍内か否かの確認
  ///
  /// @param r 赤要素
  /// @param g 緑要素
  /// @param b 青要素
  /// @return 畫素値(r, g, b)が範圍内にあるならtrue、さもなくばfalseを返す。
  bool isWithin(std::uint8_t r, std::uint8_t g, std::uint8_t b) const noexcept
  {
    return
      r >= red.min && r <= red.max
      && g >= green.min && g <= green.max
      && b >= blue.min && b <= blue.max;
  }

  /// @brief 分布情報の削除
  void clear()
  {
    red.clear();
    green.clear();
    blue.clear();
    count = 0;
  }

  /// @brief 分布情報の追加
  ///
  /// 畫素値(r, g, b)の畫素を1つ加算する。
  void add(std::uint8_t r, std::uint8_t g, std::uint8_t b) noexcept
  {
    red.add(r);
    green.add(g);
    blue.add(b);
    ++count;
  }


  /// @brief クラスタ分割
  ///
  /// *this を *this と neu に二分割する。
  /// @param[out] neu 二分割する際にその片側の情報を格納するオブジェクト
  void divide(PixelRange_& neu)
  {
    int rw = red.cutoff();
    int gw = green.cutoff();
    int bw = blue.cutoff();

    if (count == 0)
      count = 1;

    neu = *this;

    if (gw >= bw && gw >= rw) {
      green.max = green.sum / count;
      neu.green.min = green.sum / count + 1;
    }
    else if(rw >= bw && rw >= gw) {
      red.max = red.sum / count;
      neu.red.min = red.sum / count + 1;
    }
    else {
      blue.max = blue.sum / count;
      neu.blue.min = blue.sum / count + 1;
    }
  }
};


/**
 * @brief パレット生成
 *
 * 畫像からメディアンカットによりパレットを生成する。
 *
 * @param image 畫像
 * @param[out] palette 結果を格納する要素數256の配列
 */
template<class C_>
inline
void generatePalette_(const ImageBuffer<C_>& image, RgbColour palette[])
{
  PixelRange_ range[256];

  int current_range = 1;

  for (int bits = 0; bits <= 8; ++bits) {
    for (int i = 0; i < current_range; i++)
      range[i].clear();

    for (int y = 0; y < image.height(); y++) {
      for (int x = 0; x < image.width(); x++) {
        std::uint8_t r = image.pixel(x, y).red;
        std::uint8_t g = image.pixel(x, y).green;
        std::uint8_t b = image.pixel(x, y).blue;

        // r, g, bが屬するrangeを求めて、その分布情報を更新する。
        for (int i = 0; i < current_range; i++) {
          if (range[i].isWithin(r, g, b)) {
            range[i].add(r, g, b);
            break;
          }
        }
      }
    }

    if (bits == 8)
      break;

    int stop = current_range;
    for (int i = 0; i < stop; i++)
      range[i].divide(range[current_range++]);
  }

  for (int i = 0; i < 256; i++) {
    unsigned c = range[i].count == 0 ? 1u : range[i].count;
    palette[i].red = range[i].red.sum / c;
    palette[i].green = range[i].green.sum / c;
    palette[i].blue = range[i].blue.sum / c;
  }
}




/**
 * @brief パレット選擇用クラス
 */
class PaletteFinder_
{
private:
  static constexpr int NPALETTE_ = 256;
  static constexpr int SAMPLE_MAX_ = NPALETTE_ - 1;
  static constexpr int NCLASS_ = SAMPLE_MAX_ * 3 + 1;

  // POW2_[n] = n * n
  static constexpr unsigned long POW2_[NPALETTE_]
    =
  {
    0, 1, 4, 9, 16, 25, 36, 49, 64, 81, 100, 121, 144, 169, 196, 225,
    256, 289, 324, 361, 400, 441, 484, 529, 576, 625, 676, 729,
    784, 841, 900, 961, 1024, 1089, 1156, 1225, 1296, 1369, 1444, 1521,
    1600, 1681, 1764, 1849, 1936, 2025, 2116, 2209, 2304, 2401, 2500, 2601,
    2704, 2809, 2916, 3025, 3136, 3249, 3364, 3481, 3600, 3721, 3844, 3969,
    4096, 4225, 4356, 4489, 4624, 4761, 4900, 5041, 5184, 5329, 5476, 5625,
    5776, 5929, 6084, 6241, 6400, 6561, 6724, 6889, 7056, 7225, 7396, 7569,
    7744, 7921, 8100, 8281, 8464, 8649, 8836, 9025, 9216, 9409, 9604, 9801,
    10000, 10201, 10404, 10609, 10816, 11025, 11236, 11449, 11664, 11881,
    12100, 12321, 12544, 12769, 12996, 13225, 13456, 13689, 13924, 14161,
    14400, 14641, 14884, 15129, 15376, 15625, 15876, 16129, 16384, 16641,
    16900, 17161, 17424, 17689, 17956, 18225, 18496, 18769, 19044, 19321,
    19600, 19881, 20164, 20449, 20736, 21025, 21316, 21609, 21904, 22201,
    22500, 22801, 23104, 23409, 23716, 24025, 24336, 24649, 24964, 25281,
    25600, 25921, 26244, 26569, 26896, 27225, 27556, 27889, 28224, 28561,
    28900, 29241, 29584, 29929, 30276, 30625, 30976, 31329, 31684, 32041,
    32400, 32761, 33124, 33489, 33856, 34225, 34596, 34969, 35344, 35721,
    36100, 36481, 36864, 37249, 37636, 38025, 38416, 38809, 39204, 39601,
    40000, 40401, 40804, 41209, 41616, 42025, 42436, 42849, 43264, 43681,
    44100, 44521, 44944, 45369, 45796, 46225, 46656, 47089, 47524, 47961,
    48400, 48841, 49284, 49729, 50176, 50625, 51076, 51529, 51984, 52441,
    52900, 53361, 53824, 54289, 54756, 55225, 55696, 56169, 56644, 57121,
    57600, 58081, 58564, 59049, 59536, 60025, 60516, 61009, 61504, 62001,
    62500, 63001, 63504, 64009, 64516, 65025,
  };


  /// @brief 二つのRGB値の距離(の二乘)
  template<class C1_, class C2_>
  static unsigned long distance_(const C1_& c1, const C2_& c2) noexcept
  {
    return
      POW2_[std::abs(c1.red - c2.red)]
      + POW2_[std::abs(c1.green - c2.green)]
      + POW2_[std::abs(c1.blue - c2.blue)];
  }


  // SUMDIFF_[i]: RGBの總和の差がiである2つのRGB値の最小距離
  static constexpr unsigned long SUMDIFF_[NCLASS_]
    =
  {
    0, 0, 1, 3, 5, 8, 12, 16, 21, 27, 33, 40, 48, 56, 65, 75, 85, 96, 108, 120, 
    133, 147, 161, 176, 192, 208, 225, 243, 261, 280, 300, 320, 341, 363, 385, 
    408, 432, 456, 481, 507, 533, 560, 588, 616, 645, 675, 705, 736, 768, 800,
    833, 867, 901, 936, 972, 1008, 1045, 1083, 1121, 1160, 1200, 1240, 1281,
    1323, 1365, 1408, 1452, 1496, 1541, 1587, 1633, 1680, 1728, 1776, 1825,
    1875, 1925, 1976, 2028, 2080, 2133, 2187, 2241, 2296, 2352, 2408, 2465,
    2523, 2581, 2640, 2700, 2760, 2821, 2883, 2945, 3008, 3072, 3136, 3201,
    3267, 3333, 3400, 3468, 3536, 3605, 3675, 3745, 3816, 3888, 3960, 4033,
    4107, 4181, 4256, 4332, 4408, 4485, 4563, 4641, 4720, 4800, 4880, 4961,
    5043, 5125, 5208, 5292, 5376, 5461, 5547, 5633, 5720, 5808, 5896, 5985,
    6075, 6165, 6256, 6348, 6440, 6533, 6627, 6721, 6816, 6912, 7008, 7105,
    7203, 7301, 7400, 7500, 7600, 7701, 7803, 7905, 8008, 8112, 8216, 8321,
    8427, 8533, 8640, 8748, 8856, 8965, 9075, 9185, 9296, 9408, 9520, 9633,
    9747, 9861, 9976, 10092, 10208, 10325, 10443, 10561, 10680, 10800, 10920,
    11041, 11163, 11285, 11408, 11532, 11656, 11781, 11907, 12033, 12160, 12288,
    12416, 12545, 12675, 12805, 12936, 13068, 13200, 13333, 13467, 13601, 13736,
    13872, 14008, 14145, 14283, 14421, 14560, 14700, 14840, 14981, 15123, 15265,
    15408, 15552, 15696, 15841, 15987, 16133, 16280, 16428, 16576, 16725, 16875,
    17025, 17176, 17328, 17480, 17633, 17787, 17941, 18096, 18252, 18408, 18565,
    18723, 18881, 19040, 19200, 19360, 19521, 19683, 19845, 20008, 20172, 20336,
    20501, 20667, 20833, 21000, 21168, 21336, 21505, 21675, 21845, 22016, 22188,
    22360, 22533, 22707, 22881, 23056, 23232, 23408, 23585, 23763, 23941, 24120,
    24300, 24480, 24661, 24843, 25025, 25208, 25392, 25576, 25761, 25947, 26133,
    26320, 26508, 26696, 26885, 27075, 27265, 27456, 27648, 27840, 28033, 28227,
    28421, 28616, 28812, 29008, 29205, 29403, 29601, 29800, 30000, 30200, 30401,
    30603, 30805, 31008, 31212, 31416, 31621, 31827, 32033, 32240, 32448, 32656,
    32865, 33075, 33285, 33496, 33708, 33920, 34133, 34347, 34561, 34776, 34992,
    35208, 35425, 35643, 35861, 36080, 36300, 36520, 36741, 36963, 37185, 37408,
    37632, 37856, 38081, 38307, 38533, 38760, 38988, 39216, 39445, 39675, 39905,
    40136, 40368, 40600, 40833, 41067, 41301, 41536, 41772, 42008, 42245, 42483,
    42721, 42960, 43200, 43440, 43681, 43923, 44165, 44408, 44652, 44896, 45141,
    45387, 45633, 45880, 46128, 46376, 46625, 46875, 47125, 47376, 47628, 47880,
    48133, 48387, 48641, 48896, 49152, 49408, 49665, 49923, 50181, 50440, 50700,
    50960, 51221, 51483, 51745, 52008, 52272, 52536, 52801, 53067, 53333, 53600,
    53868, 54136, 54405, 54675, 54945, 55216, 55488, 55760, 56033, 56307, 56581,
    56856, 57132, 57408, 57685, 57963, 58241, 58520, 58800, 59080, 59361, 59643,
    59925, 60208, 60492, 60776, 61061, 61347, 61633, 61920, 62208, 62496, 62785,
    63075, 63365, 63656, 63948, 64240, 64533, 64827, 65121, 65416, 65712, 66008,
    66305, 66603, 66901, 67200, 67500, 67800, 68101, 68403, 68705, 69008, 69312,
    69616, 69921, 70227, 70533, 70840, 71148, 71456, 71765, 72075, 72385, 72696,
    73008, 73320, 73633, 73947, 74261, 74576, 74892, 75208, 75525, 75843, 76161,
    76480, 76800, 77120, 77441, 77763, 78085, 78408, 78732, 79056, 79381, 79707,
    80033, 80360, 80688, 81016, 81345, 81675, 82005, 82336, 82668, 83000, 83333,
    83667, 84001, 84336, 84672, 85008, 85345, 85683, 86021, 86360, 86700, 87040,
    87381, 87723, 88065, 88408, 88752, 89096, 89441, 89787, 90133, 90480, 90828,
    91176, 91525, 91875, 92225, 92576, 92928, 93280, 93633, 93987, 94341, 94696,
    95052, 95408, 95765, 96123, 96481, 96840, 97200, 97560, 97921, 98283, 98645,
    99008, 99372, 99736, 100101, 100467, 100833, 101200, 101568, 101936, 102305,
    102675, 103045, 103416, 103788, 104160, 104533, 104907, 105281, 105656,
    106032, 106408, 106785, 107163, 107541, 107920, 108300, 108680, 109061,
    109443, 109825, 110208, 110592, 110976, 111361, 111747, 112133, 112520,
    112908, 113296, 113685, 114075, 114465, 114856, 115248, 115640, 116033,
    116427, 116821, 117216, 117612, 118008, 118405, 118803, 119201, 119600,
    120000, 120400, 120801, 121203, 121605, 122008, 122412, 122816, 123221,
    123627, 124033, 124440, 124848, 125256, 125665, 126075, 126485, 126896,
    127308, 127720, 128133, 128547, 128961, 129376, 129792, 130208, 130625,
    131043, 131461, 131880, 132300, 132720, 133141, 133563, 133985, 134408,
    134832, 135256, 135681, 136107, 136533, 136960, 137388, 137816, 138245,
    138675, 139105, 139536, 139968, 140400, 140833, 141267, 141701, 142136,
    142572, 143008, 143445, 143883, 144321, 144760, 145200, 145640, 146081,
    146523, 146965, 147408, 147852, 148296, 148741, 149187, 149633, 150080,
    150528, 150976, 151425, 151875, 152325, 152776, 153228, 153680, 154133,
    154587, 155041, 155496, 155952, 156408, 156865, 157323, 157781, 158240,
    158700, 159160, 159621, 160083, 160545, 161008, 161472, 161936, 162401,
    162867, 163333, 163800, 164268, 164736, 165205, 165675, 166145, 166616,
    167088, 167560, 168033, 168507, 168981, 169456, 169932, 170408, 170885,
    171363, 171841, 172320, 172800, 173280, 173761, 174243, 174725, 175208,
    175692, 176176, 176661, 177147, 177633, 178120, 178608, 179096, 179585,
    180075, 180565, 181056, 181548, 182040, 182533, 183027, 183521, 184016,
    184512, 185008, 185505, 186003, 186501, 187000, 187500, 188000, 188501,
    189003, 189505, 190008, 190512, 191016, 191521, 192027, 192533, 193040,
    193548, 194056, 194565, 195075
  };


  // RGB成分の和を同じくするパレットにすいて片方リンクを張る。
  // index_[k]
  //   RGBの和がkであるリンクの先頭のパレット番號
  //   -1なら成分の和がkであるパレットは存在しない
  // next_[i]
  //   パレットiとリンクを同じくする次のパレットの番號
  //   -1ならパレットiはリンク終點
  int next_[NPALETTE_];
  int index_[NCLASS_];

public:
  // パレット探索リンクを構築する
  explicit PaletteFinder_(const RgbColour palette[]) noexcept
  {
    std::fill_n(index_, NCLASS_, -1);
    for (int i = 0; i < NPALETTE_; ++i) {
      int k = palette[i].red + palette[i].green + palette[i].blue;
      next_[i] = index_[k];
      index_[k] = i;
    }
  }

  // colorに最も近いパレットを求める
  template<class C_>
  std::uint8_t
  findNearest(const RgbColour palette[], const C_& color) const noexcept
  {
    // nearest: 今までに見附かつてゐる最小距離のパレットの番號 0で初期化
    // mindiff: palette[min]とcolorの距離の二乘 最大値で初期化
    std::uint8_t nearest = 0;
    unsigned long mindiff = ULONG_MAX;

    // colorのRGB成分の和
    int base = color.red + color.green + color.blue;

    // 探索對象を
    // colorとRGBの總和の差が小さいところから大きいところに擴げながら調べる
    for (int absval = 0; absval < NCLASS_; absval++) {

      // 今のところの最近パレット(nearest)との距離(mindiff)よりも
      // これから調べる對象の距離(の最小値: SUMDIFF_として與へられてゐる)の方が
      // 大きい場合は打ち切る
      if (mindiff < SUMDIFF_[absval])
        break;

      // RGBの總和が color より absval だけ大きいパレットを調べて廻る
      int p = (base + absval < NCLASS_) ? index_[base + absval] : -1;
      while (p >= 0 && p < NPALETTE_) {

        // diffはcolorとpalette[p]の距離の二乘
        unsigned long diff = distance_(color, palette[p]);

        if (diff == 0)
          return p;  // 距離0なら文句なしに終了
        else if (diff < mindiff) {
          // 最近パレットと距離の二乘を更新
          nearest = p;
          mindiff = diff;
        }
        p = next_[p]; // リンクの次に遷る
      }

      // RGBの總和が col より absval だけ小さいパレットを調べて廻る
      p = (base - absval >= 0) ? index_[base - absval] : -1;
      while (p >= 0 && p < NPALETTE_) {

        // diffはcolorとpalette[p]の距離の二乘
        unsigned long diff = distance_(color, palette[p]);

        if (diff == 0)
          return p;  // 距離0なら文句なしに終了
        else if (diff < mindiff) {
          // 最近パレットと距離の二乘を更新
          nearest = p;
          mindiff = diff;
        }
        p = next_[p]; // リンクの次に遷る
      }
    }
    return nearest;  // 最終的な最近パレット番號を返す
  }
};




/**
 * @brief 誤差分散法による減色處理
 *
 * Jarvis, Judice & Ninkeの係數を用ゐた誤差分散法により、
 * 畫像をインデックスカラー化する。
 *
 * 參考: C MAGAZINE Jun. 2000
 *
 */
template<class C_>
inline
void
decreaseColourUsingErrorDiffusion_(
  const ImageBuffer<C_>& src,
  PictureIndexed& dst,
  const PaletteFinder_& pf)
{
  constexpr int PTNW = 5; // パターン配列の幅
  constexpr int PTNH = 3; // パターン配列の高さ
  constexpr int DAREA = 2; // 分散する畫素の範圍

  constexpr int ERRPTN[PTNW * PTNH]  // 分散パターン
   = {0, 0, 0, 7, 5,  3, 5, 7, 5, 3,  1, 3, 5, 3, 1};

  int sumPtn = 0;
  for (int i = 0; i < PTNW * PTNH; i++)
    sumPtn += ERRPTN[i];

  // バッファの確保と初期化
  int mx = src.width() + DAREA * 2;
  int sum = mx * PTNH;
  
  auto rerr = std::make_unique<int[]>(sum);
  auto gerr = std::make_unique<int[]>(sum);
  auto berr = std::make_unique<int[]>(sum);
  std::fill_n(rerr.get(), sum, 0);
  std::fill_n(gerr.get(), sum, 0);
  std::fill_n(berr.get(), sum, 0);

  for (int y = 0; y < src.height(); y++) {
    for (int x = 0; x < src.width(); x++) {

      const C_& color = src.pixel(x, y);

      int adr = x + DAREA;
      int rr = color.red + rerr[adr] / sumPtn;
      int gg = color.green + gerr[adr] / sumPtn;
      int bb = color.blue + berr[adr] / sumPtn;

      int r2 = std::clamp(rr, 0, 255);
      int g2 = std::clamp(gg, 0, 255);
      int b2 = std::clamp(bb, 0, 255);

      std::uint8_t best
        = pf.findNearest(dst.paletteBuffer(), RgbColour(r2, g2, b2));

      int re = rr - dst.palette(best).red;
      int ge = gg - dst.palette(best).green;
      int be = bb - dst.palette(best).blue;


      // 誤差分散
      adr -= DAREA;
      for (int iy = 0; iy < PTNH; ++iy, adr += mx) {
        for (int ix = 0; ix < PTNW; ++ix) {
          rerr[adr + ix] += re * ERRPTN[ix + iy * PTNW];
          gerr[adr + ix] += ge * ERRPTN[ix + iy * PTNW];
          berr[adr + ix] += be * ERRPTN[ix + iy * PTNW];
        }
      }

      dst.pixel(x, y) = best;
    }

    // バッファのずらし處理
    for (int i = 0; i < mx; ++i) {
      for (int j = 0; j < PTNH - 1; ++j) {
        rerr[i + j * mx] = rerr[i + (j + 1) * mx];
        gerr[i + j * mx] = gerr[i + (j + 1) * mx];
        berr[i + j * mx] = berr[i + (j + 1) * mx];
      }
      rerr[i + (PTNH - 1) * mx] = 0;
      gerr[i + (PTNH - 1) * mx] = 0;
      berr[i + (PTNH - 1) * mx] = 0;
    }
  }
}


/**
 * @brief 單純近似による減色處理
 */
template<class C_>
inline
void
decreaseColourSimply_(
  const ImageBuffer<C_>& src,
  PictureIndexed& dst,
  const PaletteFinder_& pf)
{
  for (int y = 0; y < src.height(); ++y) {
    for (int x = 0; x < src.width(); ++x) {
      std::uint8_t best
        = pf.findNearest(dst.paletteBuffer(), src.pixel(x, y));
      dst.pixel(x, y) = best;
    }
  }
}


}// end of namespace eunomia::implement_




#endif // INCLUDE_GUARD_EUNOMIA_PICTURE_INDEXING_IMPLEMENT_H
