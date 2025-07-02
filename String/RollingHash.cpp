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
      RollingHash rh(S);                 // 前処理 O(N)
      // 1. 部分文字列一致
      if(rh.same(l1,r1,l2,r2)) ...

      // 2. パターン検索
      RollingHash hp(P);
      ull hv = hp.get(0,P.size());
      for(int i=0;i+P.size()<=S.size();++i)
          if(rh.get(i,i+P.size())==hv && S.compare(i,P.size(),P)==0) hit.push_back(i);

      // 3. LCP (suffix i,j)
      int lcp = lcp_suffix(rh,i,j,S.size());

      // 4. 回文区間判定
      RollingHash rev(string(S.rbegin(),S.rend()));
      bool pal = is_pal(rh,rev,l,r,S.size());
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

/// suffix LCP (S[i…], S[j…])
int lcp_suffix(const RollingHash &rh, int i, int j, int N)
{
  int lo = 0, hi = N - max(i, j);
  while (lo < hi)
  {
    int mid = (lo + hi + 1) / 2;
    if (rh.same(i, i + mid, j, j + mid))
      lo = mid;
    else
      hi = mid - 1;
  }
  return lo;
}

/// 区間 [l,r) が回文か判定 (前計算: forward, reverse 両 RollingHash)
bool is_pal(const RollingHash &fwd, const RollingHash &rev, int l, int r, int N)
{
  // fwd : S, rev : reversed S
  return fwd.get(l, r) == rev.get(N - r, N - l);
}

/************************************************************
◆ 典型用法 (RollingHash で LCS)
    auto [len, ia, ib] = longest_common_substr(A, B);
    // len  が 0 なら共通部分文字列なし
    // A.substr(ia,len) が最長の一例
************************************************************/
// 返り値: pair<長さ, (A側の開始位置, B側の開始位置)>
// 解が複数ある場合は最初に見つかったもの
tuple<int, int, int> longest_common_substr(const string &A, const string &B)
{
  int NA = (int)A.size(), NB = (int)B.size();
  RollingHash ha(A), hb(B);

  int lo = 0, hi = min(NA, NB), bestLen = 0;
  int bestIa = -1, bestIb = -1;

  auto existsLen = [&](int L) -> bool
  {
    unordered_map<unsigned long long, vector<int>> mp;
    mp.reserve(NA - L + 1);
    for (int i = 0; i + L <= NA; ++i)
      mp[ha.get(i, i + L)].push_back(i);

    for (int j = 0; j + L <= NB; ++j)
    {
      auto hv = hb.get(j, j + L);
      auto it = mp.find(hv);
      if (it == mp.end())
        continue;
      // 衝突チェック (必要なら)
      for (int ia : it->second)
      {
        if (A.compare(ia, L, B, j, L) == 0)
        {
          bestIa = ia;
          bestIb = j;
          return true;
        }
      }
    }
    return false;
  };

  while (lo <= hi)
  {
    int mid = (lo + hi) / 2;
    int saveIa = bestIa, saveIb = bestIb;
    if (existsLen(mid))
    { // 長さ mid の共通部分文字列が存在
      bestLen = mid;
      lo = mid + 1;
    }
    else
    {
      bestIa = saveIa;
      bestIb = saveIb; // 巻き戻す
      hi = mid - 1;
    }
  }
  return {bestLen, bestIa, bestIb}; // bestIa = -1 なら空文字
}

/************************************************************
◆ 典型用法 (最長重複部分列= 文字列 S 内で 2 回以上現れる最長の連続部分文字列)
    auto [len,pos] = longest_repeated_substr_hash(S);
    // S.substr(pos,len) が S 内で 2 回以上現れる最長部分文字列
************************************************************/
//======================================================================
// ① RollingHash + 二分探索 + ハッシュ集合   O(N log N)
//    文字列長 2e5 程度までなら十分高速。衝突が気になる場合は DoubleHash に置換。
//    返り値: {長さ, 開始位置}  (存在しなければ {0,-1})
//======================================================================
inline std::pair<int, int> longest_repeated_substr_hash(const std::string &S)
{
  using ull = unsigned long long;
  const int N = (int)S.size();
  if (N <= 1)
    return {0, -1};

  RollingHash rh(S);
  int bestLen = 0, bestPos = -1;

  // 真偽判定: 長さ L の重複部分文字列があるか
  auto existLen = [&](int L) -> bool
  {
    std::unordered_map<ull, int> mp;
    mp.reserve(N); // rehash 抑制
    for (int i = 0; i + L <= N; ++i)
    {
      ull h = rh.get(i, i + L);
      auto it = mp.find(h);
      if (it == mp.end())
      {
        mp.emplace(h, i);
      }
      else
      {
        // 衝突確認 (シングルハッシュ時のみ)
        if (S.compare(it->second, L, S, i, L) == 0)
        {
          bestPos = it->second; // 代表位置を保存
          return true;
        }
      }
    }
    return false;
  };

  // 長さを二分探索
  for (int lo = 1, hi = N - 1; lo <= hi;)
  {
    int mid = (lo + hi) / 2;
    if (existLen(mid))
    { // mid が作れる
      bestLen = mid;
      lo = mid + 1;
    }
    else
      hi = mid - 1;
  }
  return {bestLen, bestPos};
}

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
