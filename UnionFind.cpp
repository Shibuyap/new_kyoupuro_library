#pragma once
#include <bits/stdc++.h>
using namespace std;

/**
 * UnionFind（Disjoint Set Union, DSU）
 *  ・サイズ・経路圧縮付き
 *  ・0-indexed
 *  ・unite(u, v)   : u と v の属する集合をマージし，マージ成功なら true
 *  ・same(u, v)    : 同じ集合なら true
 *  ・leader(v)     : v が属する集合代表を返す
 *  ・size(v)       : v が属する集合の要素数
 *  ・groups()      : 全連結成分を vector<vector<int>> で返す（必要なときだけ）
 */
struct UnionFind
{
  vector<int> parent, sz;

  explicit UnionFind(int n = 0) { init(n); }

  void init(int n)
  {
    parent.resize(n);
    sz.assign(n, 1);
    iota(parent.begin(), parent.end(), 0); // parent[i] = i
  }

  /// 根を返す（経路圧縮）
  int leader(int v)
  {
    if (parent[v] == v)
      return v;
    return parent[v] = leader(parent[v]);
  }

  /// u, v をマージ（既に同集合なら false）
  bool unite(int u, int v)
  {
    u = leader(u);
    v = leader(v);
    if (u == v)
      return false;
    if (sz[u] < sz[v])
      swap(u, v); // 重さ（要素数）が大きい方を親に
    parent[v] = u;
    sz[u] += sz[v];
    return true;
  }

  /// 同集合判定
  bool same(int u, int v) { return leader(u) == leader(v); }

  /// 要素数
  int size(int v) { return sz[leader(v)]; }

  /// すべての連結成分を返す（必要な時だけ呼ぶ: O(N α(N)))
  vector<vector<int>> groups()
  {
    int n = parent.size();
    vector<vector<int>> g(n);
    for (int v = 0; v < n; ++v)
      g[leader(v)].push_back(v);
    g.erase(remove_if(g.begin(), g.end(),
                      [](const vector<int> &vec)
                      { return vec.empty(); }),
            g.end());
    return g;
  }
};

int main()
{
  ios::sync_with_stdio(false);
  cin.tie(nullptr);
  int N, Q;
  cin >> N >> Q;
  UnionFind uf(N);
  while (Q--)
  {
    int t, u, v;
    cin >> t >> u >> v;
    if (t == 0)
    { // 辺追加
      uf.unite(u, v);
    }
    else if (t == 1)
    { // 同一集合判定
      cout << (uf.same(u, v) ? "Yes\n" : "No\n");
    }
  }
}
