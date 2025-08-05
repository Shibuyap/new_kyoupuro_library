#include <bits/stdc++.h>
using namespace std;

/*
 * Hungarian (Kuhn–Munkres) algorithm for ASSIGNMENT PROBLEM
 * --------------------------------------------------------
 * 与えられた n×n の「利益」行列 profit[i][j] (double) について、
 * 各行・各列から 1 要素ずつ選択し、総利益を最大化する。
 *
 * 返り値:
 *   pair<double, vector<int>> (最大利益, 行 i に割り当てた列 idx[i])
 *   割当が存在しない場合 idx[i] = -1
 *
 * 計算量:  O(n^3)   (n ≤ 20 なら μs〜ms オーダ)
 *
 * 実装方針:
 *   最大化 → 最小化へ変換:  cost[i][j] = maxP - profit[i][j]
 *   ※ maxP は各行の最大値を取って +α した十分大きな定数で OK
 *   double の比較誤差を避けるため EPS を使う。
 */
pair<double, vector<int>> hungarianMax(const vector<vector<double>> &profit)
{
    const double INF = 1e100;
    const double EPS = 1e-12;
    int n = (int)profit.size();
    vector<double> u(n + 1), v(n + 1); // ポテンシャル
    vector<int> p(n + 1), way(n + 1);  // p: col→row, way: col→prev col
    vector<int> assignment(n, -1);     // row i に割り当てた col

    /* --- 最大化 → 最小化 (cost) --- */
    double maxP = -INF;
    for (auto &row : profit)
        for (double x : row)
            maxP = max(maxP, x);
    vector<vector<double>> cost(n, vector<double>(n));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            cost[i][j] = maxP - profit[i][j];

    /* --- 本体 --- */
    for (int i = 1; i <= n; ++i)
    { // 行を 1-origin で管理
        p[0] = i;
        int j0 = 0; // 現在の空列
        vector<double> minv(n + 1, INF);
        vector<char> used(n + 1, false);
        do
        {
            used[j0] = true;
            int i0 = p[j0], j1 = 0;
            double delta = INF;
            for (int j = 1; j <= n; ++j)
                if (!used[j])
                {
                    double cur = cost[i0 - 1][j - 1] - u[i0] - v[j];
                    if (cur < minv[j])
                    {
                        minv[j] = cur;
                        way[j] = j0;
                    }
                    if (minv[j] < delta)
                    {
                        delta = minv[j];
                        j1 = j;
                    }
                }
            for (int j = 0; j <= n; ++j)
            {
                if (used[j])
                {
                    u[p[j]] += delta;
                    v[j] -= delta;
                }
                else
                    minv[j] -= delta;
            }
            j0 = j1;
        } while (p[j0] != 0);

        // 増加パスの逆辿り
        do
        {
            int j1 = way[j0];
            p[j0] = p[j1];
            j0 = j1;
        } while (j0);
    }

    // 結果の取り出し
    double maxSum = 0.0;
    for (int j = 1; j <= n; ++j)
    {
        int i = p[j];
        if (i)
        {
            assignment[i - 1] = j - 1;
            maxSum += profit[i - 1][j - 1];
        }
    }
    return {maxSum, assignment};
}

/* ------------------ 使い方例 ------------------ */
int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin >> n;
    vector<vector<double>> a(n, vector<double>(n));
    for (auto &row : a)
        for (double &x : row)
            cin >> x;

    auto [ans, idx] = hungarianMax(a);

    cout << fixed << setprecision(10) << "max sum = " << ans << '\n';
    for (int i = 0; i < n; ++i)
        cout << "row " << i << " -> col " << idx[i] << '\n';
}
