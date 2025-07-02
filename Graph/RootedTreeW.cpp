#pragma once
#include <bits/stdc++.h>
using namespace std;

struct Edge
{
  int to;
  int cost;
};

/*------------------------------------------------------------
  RootedTreeW
  ・無向の重み付き木 adj と root を与える。
    - parent[v] : 親頂点（根は -1）
    - depth[v]  : 辺数ベースの深さ
    - pcost[v]  : 親→v の辺重み（根は 0）
    - dist[v]   : 根→v の累積コスト (long long)
    - children[v] : 子頂点リスト
    - subtree_size[v] : 部分木サイズ
    - order : 行きがけ DFS 順
  すべて O(N) で計算。
------------------------------------------------------------*/
struct RootedTreeW
{
  int N, root;
  vector<int> parent, depth, pcost, subtree_size, order;
  vector<long long> dist;
  vector<vector<int>> children;

  explicit RootedTreeW(const vector<vector<Edge>> &adj, int root_)
      : N((int)adj.size()),
        root(root_),
        parent(N, -1),
        depth(N, 0),
        pcost(N, 0),
        dist(N, 0),
        subtree_size(N, 1),
        children(N)
  {

    order.reserve(N);
    vector<pair<int, int>> st; // (頂点, 次の探索index)
    st.emplace_back(root, 0);

    while (!st.empty())
    {
      auto [v, idx] = st.back();
      if (idx == 0)
      {
        order.push_back(v); // 行きがけ
      }

      if (idx < (int)adj[v].size())
      {
        const Edge &e = adj[v][idx++];
        st.back().second = idx; // 次に備えて idx 更新
        int to = e.to;
        if (to == parent[v])
        {
          continue; // 逆流防止
        }
        parent[to] = v;
        pcost[to] = e.cost;
        depth[to] = depth[v] + 1;
        dist[to] = dist[v] + e.cost;
        children[v].push_back(to);
        st.emplace_back(to, 0);
      }
      else
      {
        st.pop_back(); // 帰りがけ
        if (parent[v] != -1)
        {
          subtree_size[parent[v]] += subtree_size[v];
        }
      }
    }
  }
};

int main()
{
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  int N;
  cin >> N;
  vector<vector<Edge>> g(N);
  for (int i = 0; i < N - 1; ++i)
  {
    int u, v, w;
    cin >> u >> v >> w;
    g[u].push_back({v, w});
    g[v].push_back({u, w});
  }

  int root = 0;
  RootedTreeW tree(g, root);

  for (int v = 0; v < N; ++v)
  {
    cout << "v=" << v
         << " parent=" << tree.parent[v]
         << " depth=" << tree.depth[v]
         << " dist=" << tree.dist[v]
         << " pcost=" << tree.pcost[v]
         << " subtree_size=" << tree.subtree_size[v]
         << endl;
  }
}
