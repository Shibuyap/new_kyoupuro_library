/************************************************************
  Manacher – 全回文半径を O(N) で求めるアルゴリズム
  ----------------------------------------------------------
  ◆ できること                      | 時間計算量
    - 奇数中心半径配列 d1[i]        | O(N)
    - 偶数中心半径配列 d2[i]        | O(N)
    - 最長回文部分文字列 (LPS)      | O(N)  ※max(d1,d2)
    - 全回文部分文字列総数         | O(N)  ※Σ d1 + Σ d2
    - 区間 [l,r] が回文か判定       | O(1)  ※半径と中心で比較
    - 最長回文区間の開始/長さ列挙   | O(N)

  ◆ 主要関数
      manacher(s) -> pair<vector<int>, vector<int>>
        d1 : 各 i を中心とする **奇数**長回文の半径
              (長さ = 2*d1[i]-1)
        d2 : 各 i を右側中心とする **偶数**長回文の半径
              (長さ = 2*d2[i])

  ◆ 典型用法
      auto [d1, d2] = manacher(S);
      int best = 0;
      for (int i = 0; i < (int)S.size(); ++i) {
          best = max({best, 2*d1[i]-1, 2*d2[i]});
      }
      // 区間 [l,r] が回文？
      int len = r - l + 1, c, rad;
      if (len & 1) {                         // 奇数長
          c = (l + r) >> 1;
          rad = d1[c];
          bool ok = rad >= (len+1)/2;
      } else {                              // 偶数長
          c = (l + r + 1) >> 1;             // 右寄り中心
          rad = d2[c];
          bool ok = rad >= len/2;
      }
************************************************************/

#pragma once
#include <bits/stdc++.h>
using namespace std;

/*------------------------------------------------------------
  manacher(s)
    入力 : string / vector<int> などランダムアクセス可能列
    出力 : {d1, d2}
            d1[i] = 奇数回文半径
            d2[i] = 偶数回文半径
------------------------------------------------------------*/
template <typename Seq>
pair<vector<int>, vector<int>> manacher(const Seq &s)
{
  const int n = (int)s.size();
  vector<int> d1(n), d2(n);

  // ---- odd length ----
  for (int i = 0, l = 0, r = -1; i < n; ++i)
  {
    int k = (i > r) ? 1 : min(d1[l + r - i], r - i + 1);
    while (0 <= i - k && i + k < n && s[i - k] == s[i + k])
      ++k;
    d1[i] = k;
    if (i + k - 1 > r)
    {
      l = i - k + 1;
      r = i + k - 1;
    }
  }

  // ---- even length ----
  for (int i = 0, l = 0, r = -1; i < n; ++i)
  {
    int k = (i > r) ? 0 : min(d2[l + r - i + 1], r - i + 1);
    while (0 <= i - k - 1 && i + k < n && s[i - k - 1] == s[i + k])
      ++k;
    d2[i] = k;
    if (i + k - 1 > r)
    {
      l = i - k;
      r = i + k - 1;
    }
  }
  return {d1, d2};
}

int main()
{
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  string S; // 例: "abacdfgdcaba"
  cin >> S;

  auto [d1, d2] = manacher(S);

  // 最長回文部分文字列を取得
  int best_len = 0, best_l = 0;
  for (int i = 0; i < (int)S.size(); ++i)
  {
    if (2 * d1[i] - 1 > best_len)
    {
      best_len = 2 * d1[i] - 1;
      best_l = i - d1[i] + 1;
    }
    if (2 * d2[i] > best_len)
    {
      best_len = 2 * d2[i];
      best_l = i - d2[i];
    }
  }
  cout << "Longest palindrome: " << S.substr(best_l, best_len) << '\n';
}
