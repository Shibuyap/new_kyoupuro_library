/************************************************************
  KMP (Knuth–Morris–Pratt)  –  単一パターン高速検索ライブラリ
  ----------------------------------------------------------
  ◆ できること                      | 時間計算量
    - パターン出現判定 (exists)     | O(N + M)
    - 出現位置列挙 (search)         | O(N + M)
    - 出現回数カウント (count)      | O(N + M)
    - 最長 border 長 (lps.back())   | O(M)
    - 文字列の最小周期判定          | O(M)

  ◆ 主要関数
      build_lps(pat) -> vector<int>
      kmp_search(text, pat) -> vector<int>

  ◆ 典型用法
      auto hit = kmp_search(T, P);        // 全位置
      bool ok  = !hit.empty();            // 存在するか
      int per  = P.size() - build_lps(P).back();  // 最小周期
************************************************************/

#pragma once
#include <bits/stdc++.h>
using namespace std;

/*------------------------------------------------------------
  KMP (Knuth–Morris–Pratt)  Library
    - build_lps(pattern)      : 部分一致表（lps = longest proper prefix-suffix）を作成
    - kmp_search(text, pat)   : pat の出現開始インデックスを vector<int> で返す
    （出現しない場合は空ベクタ）
------------------------------------------------------------*/

// lps[i] := pat[0..i] の proper な接頭辞＝接尾辞の最長長
vector<int> build_lps(const string &pat)
{
  int m = (int)pat.size();
  vector<int> lps(m, 0);
  for (int i = 1, len = 0; i < m; ++i)
  {
    while (len > 0 && pat[i] != pat[len])
      len = lps[len - 1];
    if (pat[i] == pat[len])
      lps[i] = ++len;
  }
  return lps;
}

// 返り値 : pat が現れる開始位置（0-indexed）の一覧
vector<int> kmp_search(const string &text, const string &pat)
{
  int n = (int)text.size(), m = (int)pat.size();
  if (m == 0)
    return {0}; // 空パターンなら常にヒット
  vector<int> lps = build_lps(pat);
  vector<int> res;
  for (int i = 0, j = 0; i < n; ++i)
  {
    while (j > 0 && text[i] != pat[j])
      j = lps[j - 1];
    if (text[i] == pat[j])
    {
      if (++j == m)
      { // マッチ完了
        res.push_back(i - m + 1);
        j = lps[j - 1]; // 次のマッチ探索へ
      }
    }
  }
  return res;
}

int main()
{
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  string T, P;
  cin >> T >> P;
  auto pos = kmp_search(T, P);

  if (pos.empty())
  {
    cout << "Not found\n";
  }
  else
  {
    cout << "Occurrences: " << pos.size() << '\n';
    for (int idx : pos)
    {
      cout << idx << ' ';
    }
    cout << '\n';
  }
}
