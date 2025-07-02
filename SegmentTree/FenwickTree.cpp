#pragma once
#include <bits/stdc++.h>
using namespace std;

/*-------------------------------------------------------
  FenwickTree  (Binary Indexed Tree)
    - コンストラクタ     Fenwick<T> bit(n);
    - add(i, v)          a[i] += v
    - sum_prefix(r)      Σ_{0 ≤ i < r} a[i]
    - sum(l, r)          Σ_{l ≤ i < r} a[i]
    - lower_bound(x)     最小 idx s.t. sum_prefix(idx+1) >= x
-------------------------------------------------------*/
template <class T = long long>
struct Fenwick
{
  int n;
  vector<T> bit;

  explicit Fenwick(int n_ = 0) : n(n_), bit(n + 1, T{}) {}

  void assign(int n_)
  {
    n = n_;
    bit.assign(n + 1, T{});
  }

  // 点加算: a[idx] += val   (0-indexed)
  void add(int idx, T val)
  {
    for (++idx; idx <= n; idx += idx & -idx)
    {
      bit[idx] += val;
    }
  }

  // 区間 [0, r) の和
  T sum_prefix(int r) const
  {
    T ret{};
    for (; r > 0; r &= r - 1)
    {
      ret += bit[r];
    }
    return ret;
  }

  // 区間 [l, r) の和
  T sum(int l, int r) const
  {
    return sum_prefix(r) - sum_prefix(l);
  }

  // 累積和が val 以上になる最小インデックス (存在しなければ n)
  int lower_bound(T val) const
  {
    if (val <= T{})
    {
      return 0;
    }
    int idx = 0;
    for (int step = 1 << __lg(n); step; step >>= 1)
    {
      int nxt = idx + step;
      if (nxt <= n && bit[nxt] < val)
      {
        val -= bit[nxt];
        idx = nxt;
      }
    }
    return idx; // 0-indexed
  }
};

int main()
{
  int N, Q;
  cin >> N >> Q;
  Fenwick<long long> bit(N);

  while (Q--)
  {
    int t;
    cin >> t;
    if (t == 0)
    { // 点更新
      int idx, v;
      cin >> idx >> v;
      bit.add(idx, v);
    }
    else if (t == 1)
    { // 区間和
      int l, r;
      cin >> l >> r;
      cout << bit.sum(l, r) << '\n';
    }
    else
    { // 累積和 lower_bound
      long long x;
      cin >> x;
      cout << bit.lower_bound(x) << '\n';
    }
  }
}
