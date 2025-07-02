/************************************************************
  Z algorithm – 文字列 S に対し Z[i] = LCP(S, S[i..]) を O(N) で計算
  ----------------------------------------------------------
  ◆ できること                      | 時間計算量
    - Z 配列一括構築               | O(N)
    - 単一パターン検索 (exists)    | O(N + M)   ※ concat 方式
    - 出現位置列挙 (search)        | O(N + M)
    - 全 border 列挙              | O(N)
    - 文字列の最小周期判定         | O(N)

  ◆ 主要関数
      z_algorithm(s)               -> vector<int>
      z_search(text, pat)          -> vector<int>  // pat が現れる開始 idx

  ◆ 典型用法
      auto z  = z_algorithm(S);
      int lcp = z[i];                          // S と suffix i の長さ
      auto hit = z_search(T, P);               // 全出現
      bool ok  = !hit.empty();                 // 存在判定
      int per  = N;                            // 最小周期
      for(int k=1;k<N;++k) if(N%k==0 && z[k]+k==N){ per=k; break; }
************************************************************/

#pragma once
#include <bits/stdc++.h>
using namespace std;

/*------------------------------------------------------------
  Z algorithm  Library
    - z_algorithm(s) : Z 配列を返す
    - z_search(text, pat) : pat が text に現れる開始 idx を列挙
------------------------------------------------------------*/

// O(N) で Z 配列 (z[0]=|s|)
vector<int> z_algorithm(const string &s)
{
  int n = (int)s.size();
  vector<int> z(n);
  z[0] = n;
  for (int i = 1, l = 0, r = 0; i < n; ++i)
  {
    if (i <= r)
      z[i] = min(r - i + 1, z[i - l]);
    while (i + z[i] < n && s[z[i]] == s[i + z[i]])
      ++z[i];
    if (i + z[i] - 1 > r)
    {
      l = i;
      r = i + z[i] - 1;
    }
  }
  return z;
}

// pat 検索。返り値: 出現開始 idx (0-indexed)
vector<int> z_search(const string &text, const string &pat)
{
  if (pat.empty())
    return {0};
  string concat = pat + '$' + text; // '$' は非出現文字
  auto z = z_algorithm(concat);
  vector<int> res;
  int m = (int)pat.size();
  for (int i = m + 1; i < (int)z.size(); ++i)
    if (z[i] >= m)
      res.push_back(i - m - 1);
  return res;
}

/*--------------------------- demo ---------------------------*/
#ifdef LOCAL
int main()
{
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  string T, P; // ex) abababa  aba
  cin >> T >> P;
  auto pos = z_search(T, P);

  if (pos.empty())
  {
    cout << "Not found\n";
  }
  else
  {
    cout << "Occurrences: " << pos.size() << '\n';
    for (int idx : pos)
      cout << idx << ' ';
    cout << '\n';
  }
}
#endif
