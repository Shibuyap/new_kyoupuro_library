#include <bits/stdc++.h>
using namespace std;

// g: 隣接リスト (g[v] に v→to を列挙)
// 戻り値: トポロジカル順序 (size()==N) か, 閉路検出で空 vector
vector<int> topological_sort(const vector<vector<int>> &g)
{
    int N = (int)g.size();
    vector<int> indeg(N, 0);
    for (int v = 0; v < N; ++v)
    {
        for (int to : g[v])
        {
            ++indeg[to];
        }
    }

    queue<int> q;
    for (int v = 0; v < N; ++v)
    {
        if (indeg[v] == 0)
        {
            q.push(v);
        }
    }

    vector<int> order;
    order.reserve(N);
    while (!q.empty())
    {
        int v = q.front();
        q.pop();
        order.push_back(v);
        for (int to : g[v])
        {
            if (--indeg[to] == 0)
            {
                q.push(to);
            }
        }
    }
    return (int)order.size() == N ? order : vector<int>(); // 閉路なら空を返す
}
