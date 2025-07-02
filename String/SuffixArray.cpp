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
      SuffixArray sa(S); // 構築
      auto idx = sa.sa;      // SA 本体
      auto &L = sa.lcp;      // LCP 配列
      // パターン検索
      auto range = sa.lower_upper(P); // [l,r) がヒット区間
      bool ok = (range.first < range.second);
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
