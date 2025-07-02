/************************************************************
  Suffix Array  –  接尾辞配列＋LCP を O(N log N) で構築
  ----------------------------------------------------------
  ◆ できること (N = |S|, M = |P|)  | 時間計算量
    - SA 構築 (doubling)          | O(N log N)
    - LCP 配列 (Kasai)            | O(N)
    - 2 接尾辞の辞書順比較        | O(1)
    - 部分文字列の辞書順比較      | O(1)  ※SA+RMQ
    - 単一パターン検索 (exists)   | O(M log N)
    - 出現区間列挙 (search)       | O(M log N)
    - 最長重複部分列 (LR)         | O(N)  ※max LCP
    - 異なる部分文字列個数        | N(N+1)/2 − Σ LCP
    - 文字列集合 LCP クエリ       | O(1)  ※RMQ 前処理 O(N log N)

  ◆ 主要構造体
      struct SuffixArray {
          vector<int> sa;   // 接尾辞順位→開始位置
          vector<int> lcp;  // 隣接接尾辞 LCP
      }

  ◆ 典型用法
      SuffixArray sa(S);           // 構築
      auto &SA  = sa.sa;           // SA 本体 (長さ N)
      auto &LCP = sa.lcp;          // 隣接 LCP (長さ N-1)

      // 1. パターン検索
      auto [l,r] = sa.lower_upper(P);
      if (l<r) {               // 存在チェック
          // 出現位置列挙 (辞書順→位置順にするなら sort)
          for (int k=l;k<r;++k) cout<<SA[k]<<" ";
      }

      // 2. 2 接尾辞/部分文字列の比較
      bool less = sa.rank[i] < sa.rank[j];   // suffix 比較
      int lcp_ij = rmq(sa.rank[i], sa.rank[j]-1); // LCP(i,j)

      // 3. 文字列統計
      long long distinct = 1LL*N*(N+1)/2 - accumulate(LCP.begin(),LCP.end(),0LL);
      int longest = *max_element(LCP.begin(), LCP.end());
************************************************************/

#pragma once
#include <bits/stdc++.h>
using namespace std;

struct SuffixArray
{
  vector<int> sa, rank, lcp;

  /*------------ public ------------*/
  template <class Seq>
  explicit SuffixArray(const Seq &s)
  {
    build_sa(s);
    build_lcp(s);
  }

  // パターン P が現れる SA 区間 [l, r) を返す
  template <class Seq>
  pair<int, int> lower_upper(const Seq &pat) const
  {
    auto cmp = [&](const Seq &P, int sufPos)
    {
      int i = 0, n = (int)P.size(), N = (int)rank.size();
      while (i < n && sufPos + i < N)
      {
        if (P[i] != _char(sufPos + i))
          return P[i] < _char(sufPos + i);
        ++i;
      }
      return i != n;
    };
    int n = (int)sa.size();
    int l = lower_bound(sa.begin(), sa.end(), pat, cmp) - sa.begin();
    int r = upper_bound(sa.begin(), sa.end(), pat,
                        [&](const Seq &P, int sufPos)
                        { return !cmp(P, sufPos) && !_eq(P, sufPos); }) -
            sa.begin();
    return {l, r};
  }

private:
  /*------------ helpers ------------*/
  inline char _char(int x) const { return _S[x]; }
  bool _eq(const string &P, int suf) const
  {
    int n = (int)P.size(), N = (int)_S.size();
    if (suf + n > N)
      return false;
    for (int i = 0; i < n; ++i)
      if (P[i] != _S[suf + i])
        return false;
    return true;
  }

  string _S;

  template <class Seq>
  void build_sa(const Seq &s)
  {
    _S.assign(s.begin(), s.end());
    int n = (int)s.size();
    sa.resize(n);
    rank.resize(n);
    iota(sa.begin(), sa.end(), 0);
    for (int i = 0; i < n; ++i)
      rank[i] = (unsigned char)s[i];

    vector<int> tmp(n);
    for (int k = 1;; k <<= 1)
    {
      auto cmp = [&](int i, int j)
      {
        if (rank[i] != rank[j])
          return rank[i] < rank[j];
        int ri = i + k < n ? rank[i + k] : -1;
        int rj = j + k < n ? rank[j + k] : -1;
        return ri < rj;
      };
      sort(sa.begin(), sa.end(), cmp);
      tmp[sa[0]] = 0;
      for (int i = 1; i < n; ++i)
        tmp[sa[i]] = tmp[sa[i - 1]] + cmp(sa[i - 1], sa[i]);
      rank.swap(tmp);
      if (rank[sa.back()] == n - 1)
        break;
    }
  }

  template <class Seq>
  void build_lcp(const Seq &s)
  {
    int n = (int)sa.size();
    lcp.assign(n - 1, 0);
    int h = 0;
    for (int i = 0; i < n; ++i)
    {
      int r = rank[i];
      if (r == 0)
        continue;
      int j = sa[r - 1];
      while (i + h < n && j + h < n && s[i + h] == s[j + h])
        ++h;
      lcp[r - 1] = h;
      if (h)
        --h;
    }
  }
};

/************************************************************
◆ 典型用法 (追記: RMQ)
    SuffixArray sa(S);
    RMQ rmq(sa.lcp);                 // LCP の RMQ 構築

    // LCP(suffix i, suffix j)
    auto lcp = [&](int i,int j){
        int ri=sa.rank[i], rj=sa.rank[j];
        if(ri>rj) swap(ri,rj);
        return rmq.query(ri, rj-1);
    };

    // 部分文字列比較
    bool less = less_substr(i,len1, j,len2);
************************************************************/
struct RMQ
{
  const vector<int> &a;   // 参照保持
  vector<vector<int>> st; // st[k][i] = 区間 [i, i+2^k-1] の min
  RMQ(const vector<int> &v) : a(v)
  {
    int n = a.size();
    int K = 32 - __builtin_clz(n); // ⌈log2 n⌉
    st.assign(K, vector<int>(n));
    st[0] = a;
    for (int k = 1; k < K; ++k)
      for (int i = 0; i + (1 << k) <= n; ++i)
        st[k][i] = min(st[k - 1][i],
                       st[k - 1][i + (1 << (k - 1))]);
  }
  // 最小値を返す（添字を返す実装にしても可）
  int query(int l, int r) const
  {
    int k = 31 - __builtin_clz(r - l + 1); // floor(log2 len)
    return min(st[k][l], st[k][r - (1 << k) + 1]);
  }
};

/// 接尾辞 i, j の LCP 長を返す（S[i…], S[j…] の最長共通接頭辞長）
inline int lcp_suffix(const SuffixArray &sa,
                      const RMQ &rmq,
                      int i, int j)
{
  if (i == j)
    return (int)sa._S.size() - i; // 同じ接尾辞
  int ri = sa.rank[i], rj = sa.rank[j];
  if (ri > rj)
    std::swap(ri, rj);
  return rmq.query(ri, rj - 1); // LCP 配列上の min
}

/// 部分文字列 S[i, i+len1) と S[j, j+len2) を辞書順で比較
///   true  : 前者 < 後者
///   false : 前者 ≥ 後者
inline bool less_substr(const SuffixArray &sa,
                        const RMQ &rmq,
                        const std::string &S,
                        int i, int len1,
                        int j, int len2)
{
  // 同じ接尾辞上に完全一致する場合
  if (i == j)
    return len1 < len2;

  int ri = sa.rank[i], rj = sa.rank[j];
  if (ri > rj)
    std::swap(ri, rj);
  int lcp = rmq.query(ri, rj - 1); // 共通接頭辞長
  int common = std::min({lcp, len1, len2});

  if (common == len1 || common == len2)
    return len1 < len2; // どちらかが prefix

  return S[i + common] < S[j + common]; // 次の文字で判定
}

/************************************************************
◆ 典型用法 (最長重複部分列= 文字列 S 内で 2 回以上現れる最長の連続部分文字列)
    SuffixArray sa(S);
    auto [len,pos] = longest_repeated_substr_sa(sa);
************************************************************/
//======================================================================
// ② SuffixArray + 最大 LCP         O(N)
//    メモリに余裕があれば、こちらが最速・衝突ゼロ。
//    返り値: {長さ, 開始位置}  (存在しなければ {0,-1})
//======================================================================
inline std::pair<int, int> longest_repeated_substr_sa(const SuffixArray &sa)
{
  int best = 0, idx = -1;
  for (size_t k = 0; k < sa.lcp.size(); ++k)
    if (sa.lcp[k] > best)
    {
      best = sa.lcp[k];
      idx = sa.sa[k]; // 左側 suffix の開始位置
    }
  return {best, idx};
}

int main()
{
  ios::sync_with_stdio(false);
  cin.tie(nullptr);
  string S;
  cin >> S;
  SuffixArray sa(S);

  int best = 0, idx = 0;
  for (int i = 0; i < (int)sa.lcp.size(); ++i)
    if (sa.lcp[i] > best)
    {
      best = sa.lcp[i];
      idx = sa.sa[i];
    }
  cout << "Longest repeat: " << S.substr(idx, best) << "\n";
}
