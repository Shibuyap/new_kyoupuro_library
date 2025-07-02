#pragma once
#include <bits/stdc++.h>
using namespace std;

/*-----------------------------------------------
  RootedTree
  ・無向木（隣接リスト adj）と root を与えると、
    - parent[v]      : 親頂点（根は -1）
    - depth[v]       : 根からの深さ
    - order          : 根からの DFS 順
    - children[v]    : 子頂点の一覧
    - subtree_size[v]: 部分木サイズ
    を O(N) で求めて保持する軽量構造体。
------------------------------------------------*/
struct RootedTree
{
  int N, root;
  vector<int> parent, depth, order, subtree_size;
  vector<vector<int>> children;

  explicit RootedTree(const vector<vector<int>> &adj, int root_)
      : N((int)adj.size()),
        root(root_),
        parent(N, -1),
        depth(N, 0),
        subtree_size(N, 1),
        children(N)
  {
    order.reserve(N);
    vector<pair<int, int>> st; // (頂点, 次の探索index)
    st.emplace_back(root, 0);
    parent[root] = -1;
    while (!st.empty())
    {
      auto [v, idx] = st.back();
      if (idx == 0)
      {
        order.push_back(v); // 行きがけ
      }
      if (idx < (int)adj[v].size())
      {
        int to = adj[v][idx++];
        st.back().second = idx; // 次に備えて idx を更新
        if (to == parent[v])
        {
          continue; // 逆流防止
        }
        parent[to] = v;
        depth[to] = depth[v] + 1;
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
  int N;
  cin >> N;
  vector<vector<int>> g(N);
  for (int i = 0; i < N - 1; ++i)
  {
    int u, v;
    cin >> u >> v;
    g[u].push_back(v);
    g[v].push_back(u);
  }

  int root = 0;
  RootedTree tree(g, root);

  for (int v = 0; v < N; ++v)
  {
    cout << v << " : parent=" << tree.parent[v] << " depth=" << tree.depth[v] << endl;
  }
}
