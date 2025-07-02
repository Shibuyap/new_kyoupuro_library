/************************************************************
  Rolling Hash  –  多項式ハッシュで部分文字列を O(1) 照合
  ----------------------------------------------------------
  ◆ できること (N = |S|, M = |P|)     | 時間計算量
    - ハッシュ前処理 build()          | O(N)
    - 部分文字列ハッシュ get(l,r)     | O(1)   ※r exclusive
    - 2 部分文字列の完全一致判定      | O(1)
    - 単一パターン検索 (exists)       | O(N+M) ※hash & 比較
    - LCP/LCS を二分＋hash で高速化   | O(log N)
    - 最長重複部分列 (Rabin–Karp)     | O(N log N)
    - 回文判定 (左右 hash 比較)       | O(1) / 区間

  ◆ 主要構造体
      struct RollingHash {
          RollingHash(string s, uint64_t base = FIXED);
          uint64_t get(int l,int r) const;          // 半開区間 [l,r)
          bool     same(int l1,int r1,int l2,int r2) const;
      }

  ◆ 典型用法
      RollingHash rh(S);                    // 前処理 O(N)
      if(rh.same(l1,r1,l2,r2)) ...          // 部分文字列一致
      // パターン検索
      uint64_t hp = RollingHash(P).get(0,P.size());
      for(int i=0;i+M<=N;++i)
          if(rh.get(i,i+M)==hp && S.substr(i,M)==P) hit.push_back(i);
************************************************************/

#pragma once
#include <bits/stdc++.h>
using namespace std;

/*------------------------------------------------------------
  RollingHash  (single 64-bit mod 2^64 自然オーバーフロー版)
    * 桁あふれをそのまま利用するため mod 計算が不要で速い
    * 64 ビット衝突を嫌う場合は ↓ の DOUBLE_HASH を有効化
------------------------------------------------------------*/
struct RollingHash
{
  using ull = unsigned long long;
  static constexpr ull FIXED_BASE = 100000007ull; // 好きな奇数
  static ull base;

  vector<ull> pow, pref;
  RollingHash() = default;

  RollingHash(const string &s, ull b = FIXED_BASE) { build(s, b); }

  void build(const string &s, ull b = FIXED_BASE)
  {
    base = b;
    int n = (int)s.size();
    pow.resize(n + 1);
    pref.resize(n + 1);
    pow[0] = 1;
    for (int i = 0; i < n; ++i)
    {
      pow[i + 1] = pow[i] * base;
      pref[i + 1] = pref[i] * base + (unsigned char)s[i] + 1; // +1 で '0' 区別
    }
  }

  // 半開区間 [l,r) のハッシュ
  ull get(int l, int r) const
  {
    return pref[r] - pref[l] * pow[r - l];
  }

  // 2 部分文字列 [l1,r1), [l2,r2) が一致するか
  bool same(int l1, int r1, int l2, int r2) const
  {
    return (r1 - l1) == (r2 - l2) && get(l1, r1) == get(l2, r2);
  }
};
RollingHash::ull RollingHash::base = RollingHash::FIXED_BASE;

/*----------- もし衝突耐性を高めたいなら二重ハッシュ ---------*/
// #define DOUBLE_HASH
#ifdef DOUBLE_HASH
struct DoubleHash
{
  static constexpr uint64_t MOD1 = 1000000007ULL;
  static constexpr uint64_t MOD2 = 1000000009ULL;
  uint64_t base;
  vector<uint64_t> pow1, pow2, pre1, pre2;

  DoubleHash(const string &s, uint64_t b = 911382323ULL) { build(s, b); }

  void build(const string &s, uint64_t b)
  {
    base = b;
    int n = s.size();
    pow1.assign(n + 1, 1);
    pow2.assign(n + 1, 1);
    pre1.assign(n + 1, 0);
    pre2.assign(n + 1, 0);
    for (int i = 0; i < n; ++i)
    {
      pow1[i + 1] = pow1[i] * base % MOD1;
      pow2[i + 1] = pow2[i] * base % MOD2;
      pre1[i + 1] = (pre1[i] * base + s[i] + 1) % MOD1;
      pre2[i + 1] = (pre2[i] * base + s[i] + 1) % MOD2;
    }
  }
  pair<uint64_t, uint64_t> get(int l, int r) const
  {
    uint64_t x1 = (pre1[r] + MOD1 - pre1[l] * pow1[r - l] % MOD1) % MOD1;
    uint64_t x2 = (pre2[r] + MOD2 - pre2[l] * pow2[r - l] % MOD2) % MOD2;
    return {x1, x2};
  }
  bool same(int l1, int r1, int l2, int r2) const
  {
    return (r1 - l1) == (r2 - l2) && get(l1, r1) == get(l2, r2);
  }
};
#endif

int main()
{
  ios::sync_with_stdio(false);
  cin.tie(nullptr);
  string S, P;
  cin >> S >> P;
  RollingHash rh(S);
  RollingHash hp(P);
  auto target = hp.get(0, P.size());

  vector<int> pos;
  for (int i = 0; i + P.size() <= S.size(); ++i)
    if (rh.get(i, i + P.size()) == target && S.compare(i, P.size(), P) == 0)
      pos.push_back(i);

  if (pos.empty())
    cout << "Not found\n";
  else
  {
    cout << "Occurrences: " << pos.size() << "\n";
    for (int x : pos)
      cout << x << " ";
    cout << "\n";
  }
}
